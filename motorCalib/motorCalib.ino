#include <Servo.h>
#include <Wire.h>
Servo esc;
int motorPin = 9;

void setup(){
  Serial.begin(9600);
  esc.attach(motorPin);
  esc.write(180);
  delay(10000);
  esc.write(0);
  delay(10000);
  esc.write(90);
  delay(10000);
  Serial.println("done");
}

void loop(){
  
}

