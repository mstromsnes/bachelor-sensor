import processing.serial.*;

Serial myPort;
float x, y, z;
float max_x = 0;
<<<<<<< HEAD
=======
float xy_plane;

>>>>>>> 1c31effb7a5da1a10003460c3c48e9de86952597

void setup() {
  size(640,640);
 
<<<<<<< HEAD
  strokeWeight(3);
  // List all the available serial ports:
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[0], 19200);
=======
  frameRate(60);
  strokeWeight(3);
  background(102);
  pushMatrix();
  // List all the available serial ports:
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[0], 9600);
>>>>>>> 1c31effb7a5da1a10003460c3c48e9de86952597
  myPort.clear();
}


void draw() {
  
<<<<<<< HEAD
  
  while (myPort.available() > 0) {
=======
  background(102);
  translate(width/2,height/2);
  if (myPort.available() > 0) {
>>>>>>> 1c31effb7a5da1a10003460c3c48e9de86952597
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
<<<<<<< HEAD
     Accel accel = new Accel(x,y,z);
     line(320,320,accel.x+320,accel.y+320);
     //print(inBuffer);
     println(max_x);
    //print("x="); print(accel.x);
    //print(" y="); print(accel.y);
    //print(" z="); println(accel.z);
=======
      xy_plane = sqrt(x*x+y*y);
      line(0,0,10*x,10*y);
      noFill();
      ellipse(0,0,2*10*xy_plane,2*10*xy_plane);
      
      println(x);
      println(y);
      
    // print(inBuffer);
  //   println(max_x);
     
    //print("x="); print(x);
    //print(" y="); print(y);
    //print(" z="); println(z);
>>>>>>> 1c31effb7a5da1a10003460c3c48e9de86952597
    }
  }
}