#include <Servo.h>
#include <string>

// Global Variables
Servo myservo;
int pos = 0;

// Main code
void setup(){
  Serial.begin(57600);
  myservo.attach(9);
}

void loop(){
  if(Serial.available()){
    String posString = Serial.readStringUntil('\n');
    int pos = posString.toInt();
    myservo.write(pos);
//    Serial.print("You sent me: ");
//    Serial.println(pos);
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
