#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file
#include "Wire.h"

MPU6050 accel;

#define LED_PIN 13
bool led = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
long currentMicros, previousMicros, dt = 0;

// Tap Detection
const uint16_t tapThreshold = 140;
const uint8_t  sampleThreshold = 6;
const uint16_t peaceThreshold = 50;
const uint8_t peaceTimer = 5;
const uint8_t turbulenceThreshold = 16;
uint8_t peaceCounter = 0;
uint8_t tapCounter = 0;
bool tapFound = false;
bool peaceFollowed = false;
uint8_t tapTime = 0;
uint8_t turbulence[50];
uint8_t turbulenceCounter = 0;
uint8_t sumOfTurbulence = 0;

bool LED_ON = false;
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
    
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  // initialize device
    Serial.println(F("Initializing I2C devices..."));
    accel.initialize();

    // verify connection
    Serial.println(F("Testing device connections..."));
    Serial.println(accel.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

//    // wait for ready
//    Serial.println(F("\nSend any character to begin DMP programming and demo: "));
//    while (Serial.available() && Serial.read()); // empty buffer
//    while (!Serial.available());                 // wait for data
//    while (Serial.available() && Serial.read()); // empty buffer again

    // load and configure the DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = accel.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
//    accel.setXGyroOffset(220);
//    accel.setYGyroOffset(76);
//    accel.setZGyroOffset(-85);
    accel.setZAccelOffset(1908); // 1688 factory default for my test chip

    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        Serial.println(F("Enabling DMP..."));
        accel.setDMPEnabled(true);

        // enable Arduino interrupt detection
        Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = accel.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        Serial.println(F("DMP ready! Waiting for first interrupt..."));
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = accel.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
     // configure LED for output
    pinMode(LED_PIN, OUTPUT);

    memset(turbulence,0,sizeof(turbulence));
}

void loop() {
  // put your main code here, to run repeatedly:
  // if programming failed, don't try to do anything
  if(!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt) {
      
    }
    
  // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = accel.getIntStatus();
    
    // get current FIFO count
    fifoCount = accel.getFIFOCount();

      // check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
        accel.resetFIFO();
        Serial.println(F("FIFO overflow!"));

        // otherwise, check for DMP data ready interrupt (this should happen frequently)
    } else if (mpuIntStatus & 0x02) {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) fifoCount = accel.getFIFOCount();

        // read a packet from FIFO
        accel.getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        previousMicros = currentMicros;
        currentMicros = micros();
        dt = currentMicros-previousMicros;

        // display real acceleration, adjusted to remove gravity
            accel.dmpGetQuaternion(&q, fifoBuffer);
            accel.dmpGetAccel(&aa, fifoBuffer);
            accel.dmpGetGravity(&gravity, &q);
            accel.dmpGetLinearAccel(&aaReal, &aa, &gravity);
           if(!LED_ON){
               if (abs(aaReal.z) > peaceThreshold){
                  peaceCounter = 0;
                  peaceFollowed = false;
                  sumOfTurbulence = sumOfTurbulence - turbulence[turbulenceCounter];
                  turbulence[turbulenceCounter++] = 1;
                  sumOfTurbulence = sumOfTurbulence + 1;
                  if(abs(aaReal.z) > tapThreshold){
                  tapCounter++;
                  tapFound = false;
                  }
               }
               if (abs(aaReal.z) < tapThreshold){
                  if(tapCounter > 0 && tapCounter < sampleThreshold){
                    tapFound = true;
                     }
                    tapCounter = 0;
                  }
                if (abs(aaReal.z)<peaceThreshold){
                  sumOfTurbulence = sumOfTurbulence - turbulence[turbulenceCounter];
                  turbulence[turbulenceCounter++] = 0;
                }
                if(turbulenceCounter == 50) turbulenceCounter = 0;
                

               if(tapFound){
                  if(abs(aaReal.z) < peaceThreshold) peaceCounter++;
                  if(peaceCounter == peaceTimer && sumOfTurbulence < turbulenceThreshold){ 
                      peaceFollowed = true;
                      digitalWrite(LED_PIN,tapFound);
                      Serial.print("Haha");
                      LED_ON = true;
                      tapTime = 0;
                      peaceCounter = 0;
                      tapFound = false;
                  }
               }
               if(tapCounter>sampleThreshold)tapCounter = 0;
           }

            
            if(tapTime == 100){
              tapFound = false;
              peaceFollowed = false;
              digitalWrite(LED_PIN,tapFound);
              LED_ON = false;
              tapTime = 0;
              Serial.print("Hihi");
            }
            if(peaceFollowed) tapTime++;
            
            // Serial.print("areal\t");
            Serial.print(aaReal.x);
            Serial.print("\t");
            Serial.print(aaReal.y);
            Serial.print("\t");
            Serial.print(aaReal.z);
            Serial.print("\t");
            Serial.print(tapTime);
            Serial.print("\t");            
            Serial.print(tapCounter);
            Serial.print("\t");
            Serial.print(peaceCounter);
            Serial.print("\t");
            Serial.print(sumOfTurbulence);
            Serial.print("\t");
            Serial.print(1000000/dt); Serial.print("\n"); //Prints the sampling frequency
            
            }
}
