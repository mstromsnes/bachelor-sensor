
import processing.serial.*;
import java.util.Date;

Serial myPort;
float x, y, z;      // Stores the acceldata
long t, rt, currentDelay, previousDelay = 0;     // Stores time-information to deal with delay
int j = 0;
float max_x = 0;    //
float xy_plane;     // Magnitude of the xy-vector
int fr = 15;        // Initial framerate


void setup() {
  size(640,640);
  frameRate(fr);
  strokeWeight(3);
  background(102);
  pushMatrix();
  // List all the available serial ports:
  printArray(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[0], 9600);
  Date time = new Date();
  rt = time.getTime()+2000;  // Records the startup time
}


void draw() {
  Date time = new Date();
  background(102);
  translate(width/2,height/2);
  
  // Reads 4 lines from the serial port and records them in appropriate variables (not syncronised. May get y-value of a reading at most 1 later than x)
  if (myPort.available() > 0) {                    
    for (int i = 0; i < 4; i++){  
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
        if (inBuffer.startsWith("t")) {
          t = (long)float(inBuffer.substring(1));
        }
        if (x > max_x){max_x = x;}
        xy_plane = sqrt(x*x+y*y);
    }
   }
      line(0,0,-10*x,10*y);                                 //
      noFill();                                             // Draws the vector and magnitude-circle
      ellipse(0,0,2*10*xy_plane,2*10*xy_plane);             //
      j++;
      if(j == 30){
        previousDelay=currentDelay;
        j = 0;
      }
      currentDelay = time.getTime()-t-rt;
      print("Time delay: "); print(currentDelay);  // Writes time-delay to console. Real-time is not 0
      println("ms");
      if (time.getTime()-rt > 2000){
          if ( currentDelay-previousDelay > 100 ) {frameRate(++fr);}   // Checks if delay is too big and increases framerate to adjust. Don't check in the first 2 seconds
      }
      print("Framerate: "); println(fr);                    // Writes current framerate to console
  }
}