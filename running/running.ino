#include <Servo.h>
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

Servo esc,steering;
LSM9DS1 imu;

#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW

#define PRINT_CALCULATED
//#define PRINT_RAW
#define PRINT_SPEED 100 // 100 ms between prints

#define DECLINATION 1.29 // Declination (degrees) in St. Louis,MO

unsigned long lastPrint = 0; // Keep track of print time
unsigned long threshold;


int throttlePin = 6;
int steerPin = 5;
int motorPin = 9;
int servoPin = 11;
int Ledpin = 13;
boolean readyNot = 0;
int throttle;
int steer;

int delta = 100;


void setup() {
  // put your setup code here, to run once:
  
  imu.settings.device.commInterface = IMU_MODE_I2C;  //imu setting 
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  
  Serial.begin(9600);
 
  esc.attach(motorPin); // pin assignment for motor & servo
  steering.attach(servoPin);
  esc.write(90);
  delay(4000);
  pinMode(throttlePin, INPUT);
  pinMode(steerPin, INPUT);
  pinMode(Ledpin, OUTPUT);
  if (!imu.begin())
  {
    Serial.println("Failed to communicate with LSM9DS1.");
    Serial.println("Double-check wiring.");
    Serial.println("Default settings in this sketch will " \
                  "work for an out of the box LSM9DS1 " \
                  "Breakout, but may need to be modified " \
                  "if the board jumpers are.");
    while (1)
      ;
  }
  threshold = millis();
  digitalWrite(Ledpin, HIGH);
  delay(1000);
  digitalWrite(Ledpin, LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    char in = Serial.read();
    if(in == '!'){
      readyNot = 1;
    }
  }
  
  if(readyNot){
    digitalWrite(Ledpin, HIGH);
    if(millis()>threshold){
      throttle = pulseIn(throttlePin, HIGH);
      steer = pulseIn(steerPin, HIGH);
      //Serial.print("Before:    ");
      //Serial.print("throttle:");
      //Serial.println(throttle);
      //Serial.print(" steer:");
      //Serial.println(steer);*/
      throttle = get_throttle(throttle);
      steer = get_steer(steer);
      /*Serial.print("after:    ");
      Serial.print("throttle:");
      Serial.println(throttle);
      Serial.print(" steer:");
      Serial.println(steer);*/
      //Serial.print('T');
      //Serial.println(throttle);
      //Serial.print('S');
      //Serial.println(steer);
      if(throttle == 70) throttle = 90;
      esc.write(throttle);
      steering.write(steer);
      threshold = millis() + delta;
    }
    imu_read();
    imu_write();
  }
  
}

void imu_read(){
  if ( imu.gyroAvailable() )
  {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data.
    imu.readGyro();
  }
  if ( imu.accelAvailable() )
  {
    // To read from the accelerometer, first call the
    // readAccel() function. When it exits, it'll update the
    // ax, ay, and az variables with the most current data.
    imu.readAccel();
  }
  if ( imu.magAvailable() )
  {
    // To read from the magnetometer, first call the
    // readMag() function. When it exits, it'll update the
    // mx, my, and mz variables with the most current data.
    imu.readMag();
  }
}

void imu_write(){
  if ((lastPrint + PRINT_SPEED) < millis()){
    printGyro();  // Print "G: gx, gy, gz"
    printAccel(); // Print "A: ax, ay, az"
    //printMag();   // Print "M: mx, my, mz"
    // Print the heading and orientation for fun!
    // Call print attitude. The LSM9DS1's mag x and y
    // axes are opposite to the accelerometer, so my, mx are
    // substituted for each other.
    printAttitude(imu.ax, imu.ay, imu.az, 
                 -imu.my, -imu.mx, imu.mz);
    Serial.println();
    
    lastPrint = millis(); // Update lastPrint time
  }
}

void printGyro()
{
  // Now we can use the gx, gy, and gz variables as we please.
  // Either print them as raw ADC values, or calculated in DPS.
  Serial.print('G');
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcGyro helper function to convert a raw ADC value to
  // DPS. Give the function the value that you want to convert.
  /*Serial.print(imu.calcGyro(imu.gx), 2);
  Serial.print(", ");
  Serial.print(imu.calcGyro(imu.gy), 2);
  Serial.print(", ");*/
  Serial.println(imu.calcGyro(imu.gz), 2);
  //Serial.println(" deg/s");
#elif defined PRINT_RAW
  Serial.print(imu.gx);
  Serial.print(", ");
  Serial.print(imu.gy);
  Serial.print(", ");
  Serial.println(imu.gz);
#endif
}

void printAccel()
{  
  // Now we can use the ax, ay, and az variables as we please.
  // Either print them as raw ADC values, or calculated in g's.
  Serial.print('A');
#ifdef PRINT_CALCULATED
  // If you want to print calculated values, you can use the
  // calcAccel helper function to convert a raw ADC value to
  // g's. Give the function the value that you want to convert.
  Serial.print(imu.calcAccel(imu.ax), 2);
  Serial.print(", ");
  Serial.println(imu.calcAccel(imu.ay), 2);
  //Serial.print(", ");
  //Serial.print(imu.calcAccel(imu.az), 2);
  //Serial.println(" g");
#elif defined PRINT_RAW 
  Serial.print(imu.ax);
  Serial.print(", ");
  Serial.print(imu.ay);
  Serial.print(", ");
  Serial.println(imu.az);
#endif

}

void printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  
  float heading;
  if (my == 0)
    heading = (mx < 0) ? PI : 0;
  else
    heading = atan2(mx, my);
    
  heading -= DECLINATION * PI / 180;
  
  //Serial.print("Heading: "); Serial.println(heading, 2);
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;
  
  /*Serial.print("Pitch, Roll: ");
  Serial.print(pitch, 2);
  Serial.print(", ");
  Serial.println(roll, 2);*/
  Serial.print('H'); Serial.println(heading, 2);
}

int get_throttle(int t){
  if((t<1490 && t>1440) || t ==0){
    return 90;
  }else{
    if(t> 1490){
      return 105;
    }else{
      return 70;
    }
  }
}

int get_steer(int s){
  //s<1490 && s>1450)
  //83 是中间
  //55 最左
  //111 最右
  if((s<1500 && s>1440)|| s==0){
    return 83;
  }else{
    return map(s,970,2021,55,111);
  }
}


