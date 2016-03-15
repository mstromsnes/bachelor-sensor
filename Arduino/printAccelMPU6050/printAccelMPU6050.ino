#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

//#define OUTPUT_READABLE_ACCEL
#define OUTPUT_LOWPASS_ACCEL
//#define OUTPUT_MAX_VALUES
//#define OUTPUT_BINARY_ACCEL


MPU6050 accel;
//MPU6050 accel(0x69); //Uses MPU9250 instead with my wiring
long sampling_rate = 100;
int16_t unscaled_x, unscaled_y, unscaled_z;
float x,y,z,m;
float xm,ym,zm,mm = 0; // Used to record highest values
float x_m,y_m,z_m = 0; // Individual axis have negative values. Used to record highest negative

long currentMicros, previousMicros, dt = 0;


  float ax,ay,az,am; // Average values
  bool init_avg, high_event = false;
  int event_counter = 0;
  float cutoff = 50; // Cutoff frequency for lowpass filter
  float alpha;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Initializing accelerometer");
  accel.initialize();
  Serial.println("Testing connection...");
  Serial.println(accel.testConnection() ? "MPU 6050 connection successful" : "MPU6050 connection failed");
}

void loop() {
  // put your main code here, to run repeatedly:
  // read raw accel measurements from device
  accel.getAcceleration(&unscaled_x,&unscaled_y,&unscaled_z);
  
  x = (float)unscaled_x/32768.0*9.81*2;
  y = (float)unscaled_y/32768.0*9.81*2;
  z = (float)unscaled_z/32768.0*9.81*2;
  m = sqrt(pow(x,2)+pow(y,2)+pow(z,2));
  //m = (float)unscaled_m/32768.0*9.81*2;
  previousMicros = currentMicros;
  currentMicros = micros();
  dt = currentMicros-previousMicros;

    if (m > 12 and m > mm) {
      mm=m;
      high_event = true;
      event_counter = 0;
    }
    if (high_event) ++event_counter;
    if (event_counter > 80) {
      event_counter = 0;
      high_event = false;
      mm = m;
    }
    
    if (x > xm) xm=x;
    if (y > ym) ym=y;
    if (z > zm) zm=z;
    if (x < x_m) x_m=x;
    if (y < y_m) y_m=y;
    if (y < z_m) z_m=z;
  
   #ifdef OUTPUT_READABLE_ACCEL
        // display tab-separated accel/gyro x/y/z values
        //Serial.print("a/g:\t");
        Serial.print(x); Serial.print("\t");
        Serial.print(y); Serial.print("\t");
        Serial.print(z); Serial.print("\t");
      
        Serial.print(m);   Serial.print("\t");
        Serial.print(mm);  Serial.print("\n");
        //Serial.print(1000000/dt); Serial.print("\n");  //Prints the sampling frequency 
        //Serial.println("Add garbage data");
    #endif

    #ifdef OUTPUT_BINARY_ACCEL
        Serial.write((uint8_t)(x >> 8)); Serial.write((uint8_t)(x & 0xFF));
        Serial.write((uint8_t)(y >> 8)); Serial.write((uint8_t)(y & 0xFF));
        Serial.write((uint8_t)(z >> 8)); Serial.write((uint8_t)(z & 0xFF));
    #endif

    #ifdef OUTPUT_MAX_VALUES
      
        Serial.print(xm); Serial.print("\t");
        Serial.print(ym); Serial.print("\t");
        Serial.print(zm); Serial.print("\t");
        Serial.print(mm); Serial.print("\n");
    #endif

    #ifdef OUTPUT_LOWPASS_ACCEL
        alpha = TWO_PI*dt/1000000.0*cutoff/(1+TWO_PI*dt/1000000*cutoff);
        if (!init_avg){
          ax = x;
          ay = y;
          az = z;
          am = m;
          init_avg = true;
        } else{
          ax = ax + alpha * (x-ax);
          ay = ay + alpha * (y-ay);
          az = az + alpha * (z-az);
          am = am + alpha * (m-am);
        }
          Serial.print(ax); Serial.print("\t");
          Serial.print(ay); Serial.print("\t");
          //Serial.print(alpha);
          Serial.print(az); Serial.print("\t");
          Serial.print(am); Serial.print("\t");
          Serial.print(mm);  Serial.print("\t");
          Serial.print(1000000/dt); Serial.print("\t"); //Prints the sampling frequency
          Serial.println(sampling_rate); 

    #endif
 delay(1000/sampling_rate);
}
