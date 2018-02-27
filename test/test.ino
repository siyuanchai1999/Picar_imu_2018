#include <Servo.h>

Servo esc,steering;
int throttlePin = 6;
int steerPin = 5;
//?
int motorPin = 3;
int servoPin = 9;
int throttle;
int steer;

int delta = 500;
long threshold;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  esc.attach(motorPin);
  steering.attach(servoPin);
  pinMode(throttlePin, INPUT);
  pinMode(steerPin, INPUT);
  Serial.println("ready");
  threshold = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis()>threshold){
    throttle = pulseIn(throttlePin, HIGH);
    steer = pulseIn(steerPin, HIGH);
    Serial.print("Before:    ");
    Serial.print("throttle:");
    Serial.print(throttle);
    Serial.print(" steer:");
    Serial.println(steer);
    throttle = get_throttle(throttle);
    steer = get_steer(steer);
    Serial.print("after:    ");
    Serial.print("throttle:");
    Serial.print(throttle);
    Serial.print(" steer:");
    Serial.println(steer);
    esc.write(throttle);
    steering.write(steer);
    threshold = millis() + delta;
  }

  
}

int get_throttle(int t){
  if((t<1490 && t>1440) || t ==0){
    return 90;
  }else{
    if(t> 1490){
      return 100;
    }else{
      return 70;
    }
  }
}

int get_steer(int s){
  //(s<1490 && s>1450)
  //83 middle
  //55 leftmost
  //111 rightmost
  if((s<1496 && s>1440)|| s==0){
    return 83;
  }else{
    return map(s,970,2021,55,111);
  }
}

