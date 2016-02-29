import processing.serial.*;

Serial myPort;
float x, y, z;
float max_x = 0;

void setup() {
  size(640,640);
 
  strokeWeight(3);
  // List all the available serial ports:
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[0], 19200);
  myPort.clear();
}


void draw() {
  
  
  while (myPort.available() > 0) {
    String inBuffer = myPort.readStringUntil(10); // 10 is Linefeed in ASCII
    if (inBuffer != null) {
      if (inBuffer.startsWith("x")) {
        x = float(inBuffer.substring(1));
      }
      if (inBuffer.startsWith("y")) {
        y = float(inBuffer.substring(1));
      }
      if (inBuffer.startsWith("z")) {
        z = float(inBuffer.substring(1));
      }
      if (x > max_x){max_x = x;}
     Accel accel = new Accel(x,y,z);
     line(320,320,accel.x+320,accel.y+320);
     //print(inBuffer);
     println(max_x);
    //print("x="); print(accel.x);
    //print(" y="); print(accel.y);
    //print(" z="); println(accel.z);
    }
  }
}