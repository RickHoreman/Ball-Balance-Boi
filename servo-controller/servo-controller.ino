#include <Servo.h>
#include <string>

// Global Variables
Servo servo0;
Servo servo1;
Servo servo2;

// Main code
void setup(){
  Serial.begin(115200);
  servo0.attach(9);
  servo1.attach(8);
  servo2.attach(7);
}

void loop(){
  if(Serial.available()){
    String posString = Serial.readStringUntil('\n');
    int i0 = posString.indexOf(' ');
    int i1 = posString.indexOf(' ', i0+1);
    int i2 = posString.indexOf(' ', i1+1);
    float pos0 = posString.substring(0, i0).toFloat();
    float pos1 = posString.substring(i0+1, i1).toFloat();
    float pos2 = posString.substring(i1+1, i2).toFloat();
    servo0.write(pos0);
    servo1.write(pos1);
    servo2.write(pos2);
  }
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);              // tell servo to go to position in variable 'pos'
//    delay(15);                       // waits 15ms for the servo to reach the position
//  }
}
