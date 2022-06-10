#include <Servo.h>
#include <string>

// Global Variables
Servo servo0;
Servo servo1;
Servo servo2;
int i = 0;

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
    int pos = posString.toInt();
    if(i == 0){
      servo0.write(pos);
    }else if(i == 1){
      servo1.write(pos);
    }else if(i==2){
      servo2.write(pos);
    }
    
    Serial.print(i);
    Serial.print(" : ");
    Serial.println(pos);
    
    i++;
    i = i%3;

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
