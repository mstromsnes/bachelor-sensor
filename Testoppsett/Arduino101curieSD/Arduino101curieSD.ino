/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/*
   This sketch example demonstrates how the BMI160 on the
   Intel(R) Curie(TM) module can be used to read accelerometer data
*/

#include "CurieIMU.h"
#include "SD.h"


#define LED_PIN 13
#define breadLED 4
// ================================================================
// ===                      SD CARD STUFF                       ===
// ================================================================

File logfile;

#define chipSelect 10
// read a Hex value and return the decimal equivalent


// blink out an error code
void error(uint8_t errno) {
  /*
    if (SD.errorCode()) {
    putstring("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
    }
  */
  while (1) {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
      delay(100);
    }
    for (i = errno; i < 10; i++) {
      delay(200);
    }
  }
}

int ax, ay, az;         // accelerometer values
int gx, gy, gz;         // gyrometer values

int calibrateOffsets = 1; // int to determine whether calibration takes place or not

long lastFlushTime, timeSinceBoot, ledTimer = millis();

void setup() {
  Serial.begin(115200); // initialize Serial communication
 // while (!Serial);    // wait for the serial port to open

  // initialize device
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  pinMode(breadLED, OUTPUT);
  
  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
  CurieIMU.interrupts(CURIE_IMU_DATA_READY);

  if (calibrateOffsets == 1) {
      Serial.println("Internal sensor offsets BEFORE calibration...");
      Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
      Serial.print("\t"); // -76
      Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
      Serial.print("\t"); // -235
      Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
      Serial.print("\t"); // 168
      Serial.print(CurieIMU.getGyroOffset(X_AXIS));
      Serial.print("\t"); // 0
      Serial.print(CurieIMU.getGyroOffset(Y_AXIS));
      Serial.print("\t"); // 0
      Serial.println(CurieIMU.getGyroOffset(Z_AXIS));

      digitalWrite(breadLED,HIGH);
  
      // To manually configure offset compensation values,
      // use the following methods instead of the autoCalibrate...() methods below
      //CurieIMU.setAccelerometerOffset(X_AXIS,128);
      //CurieIMU.setAccelerometerOffset(Y_AXIS,-4);
      //CurieIMU.setAccelerometerOffset(Z_AXIS,127);
      //CurieIMU.setGyroOffset(X_AXIS,129);
      //CurieIMU.setGyroOffset(Y_AXIS,-1);
      //CurieIMU.setGyroOffset(Z_AXIS, 254);
  
      Serial.println("About to calibrate. Make sure your board is stable and upright");
      delay(5000);
      digitalWrite(breadLED,LOW);
      // The board must be resting in a horizontal position for
      // the following calibration procedure to work correctly!
      Serial.print("Starting Gyroscope calibration and enabling offset compensation...");
      CurieIMU.autoCalibrateGyroOffset();
      Serial.println(" Done");
  
      Serial.print("Starting Acceleration calibration and enabling offset compensation...");
      CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
      CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
      CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
      Serial.println(" Done");
  
      Serial.println("Internal sensor offsets AFTER calibration...");
      Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
      Serial.print("\t"); // -76
      Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
      Serial.print("\t"); // -2359
      Serial.print(CurieIMU.getAccelerometerOffset(Z_AXIS));
      Serial.print("\t"); // 1688
      Serial.print(CurieIMU.getGyroOffset(X_AXIS));
      Serial.print("\t"); // 0
      Serial.print(CurieIMU.getGyroOffset(Y_AXIS));
      Serial.print("\t"); // 0
      Serial.println(CurieIMU.getGyroOffset(Z_AXIS));
    }
    for (int i = 0; i < 3; i++){
      digitalWrite(breadLED,HIGH);
      delay(1000);
      digitalWrite(breadLED,LOW);
      delay(500);
      
  //    Serial.println("Is Data-ready interrupt enabled? \t" + String(CurieIMU.interruptEnabled(CURIE_IMU_DATA_READY)));
    }
    Serial.println("Accelerometer Data Rate\t" + String(CurieIMU.getAccelerometerRate()));
    Serial.println("Gyroscope Data Rate\t" + String(CurieIMU.getGyroRate()));
   // configure LED for output
  pinMode(LED_PIN, OUTPUT);


  // ================================================================
  // ===                      SD CARD STUFF                       ===
  // ================================================================
  pinMode(10, OUTPUT);
  // see if the card is present and can be initialized:
//  if (!SD.begin(chipSelect, 11, 12, 13)) {
  if (!SD.begin(chipSelect)) {      // if you're using an UNO, you can use this line instead
    Serial.println("Card init. failed!");
    error(2);
  }
  char filename[15];
  strcpy(filename, "CURIE00.TXT");
  for (uint8_t i = 0; i < 100; i++) {
    filename[5] = '0' + i / 10;
    filename[6] = '0' + i % 10;
    // create if does not exist, do not open existing, write, sync after write
    if (! SD.exists(filename)) {
      break;
    }
  }
  char ratefile[15];
  strcpy(ratefile,filename);
  ratefile[0] = 'R'; ratefile[1] = 'A'; ratefile[2] = 'T'; ratefile[3] = 'E'; ratefile[4] = 'S';
  File ratelogfile;
  ratelogfile = SD.open(ratefile, FILE_WRITE);
  if ( ! ratelogfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    error(3);
  }
  ratelogfile.print("Accelerometer Sampling Rate:\t");
  ratelogfile.println(CurieIMU.getAccelerometerRate());
  ratelogfile.print("Accelerometer Range (G):\t");
  ratelogfile.println(CurieIMU.getAccelerometerRange());
  ratelogfile.print("Gyroscope Sampling Rate:\t");
  ratelogfile.println(CurieIMU.getGyroRate());
  ratelogfile.print("Gyroscope Range (degrees pr second):\t");
  ratelogfile.println(CurieIMU.getGyroRange());
  ratelogfile.println("Format in logfile:");
  ratelogfile.println("millis_since_boot,ax,ay,az,gx,gy,gz");
  ratelogfile.println("All units are LSB");
  ratelogfile.close();
  
  logfile = SD.open(filename, FILE_WRITE);
  if ( ! logfile ) {
    Serial.print("Couldnt create ");
    Serial.println(filename);
    error(3);
  }
  Serial.print("Writing to ");
  Serial.println(filename);
  
  digitalWrite(breadLED,HIGH);
  delay(10000);
  digitalWrite(breadLED,LOW);
  delay(2000);
  
}

void loop() {
  if(CurieIMU.getInterruptStatus(CURIE_IMU_DATA_READY)){
  // read raw accelerometer measurements from device
  CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);
  timeSinceBoot = millis();
  
  String logstring = String(timeSinceBoot)+"," + String(ax) + "," + String(ay) + "," +String(az) + "," +String(gx) + "," +String(gy) + "," +String(gz);
  // display comma-separated accelerometer and gyroscope x/y/z values
  Serial.println(logstring);
  logfile.println(logstring);
  }
  if (timeSinceBoot > lastFlushTime+1000){
    logfile.flush();
    lastFlushTime = millis();
  }
  if (timeSinceBoot > ledTimer + 5000){
    digitalWrite(breadLED,HIGH);
    logfile.println("# " + String(millis()) + " LED ON");
    ledTimer = millis();
  }
  if((timeSinceBoot > ledTimer+600) & digitalRead(breadLED)){
    digitalWrite(breadLED,LOW);
    logfile.println("# " + String(millis()) + " LED OFF");
  }


}

