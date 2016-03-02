import processing.serial.*;

Serial myPort;
float x, y, z;
float max_x = 0;
float xy_plane;


void setup() {
  size(640,640);
 
  frameRate(60);
  strokeWeight(3);
  background(102);
  pushMatrix();
  // List all the available serial ports:
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[0], 9600);
  myPort.clear();
}


void draw() {
  
  background(102);
  translate(width/2,height/2);
  if (myPort.available() > 0) {
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
    }
  }
}