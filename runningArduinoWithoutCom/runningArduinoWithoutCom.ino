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
#define PRINT_SPEED 50 // 100 ms between prints

#define DECLINATION 1.29 // Declination (degrees) in St. Louis,MO
float recentFive[5];
float mxFive[5];
float myFive[5];
float axFive[5];
float ayFive[5];
long index = 0;
float headingMean =0;
float axMean = 0;
float ayMean = 0;
float mxMean = 0;
float myMean = 0;
int numAverage = 5;


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

int delta = 50;


void setup() {
  // put your setup code here, to run once:
  
  imu.settings.device.commInterface = IMU_MODE_I2C;  //imu setting 
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
  
  Serial.begin(115200);
 
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
      digitalWrite(Ledpin, HIGH);
    }
  }
  
  if(readyNot){
    if(millis()>threshold){
      //unsigned long startTime = millis();
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
      Serial.print('T');
      Serial.println(throttle);
      
      if(throttle == 70){
        readyNot = 0;
        digitalWrite(Ledpin,LOW);
      }
      esc.write(throttle);
      steering.write(steer);
      threshold = millis() + delta;
      imu_read();
      imu_write();
      
    }
    
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
  //if ((lastPrint + PRINT_SPEED) < millis()){
    //printGyro();  // Print "G: gx, gy, gz"
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

void printGyro()
{
  // Now we can use the gx, gy, and gz variables as we please.
  // Either print them as raw ADC values, or calculated in DPS.
 // Serial.print('G');
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
  float axOffset, ayOffset,axResult, ayResult;
  float extraOffset = -0.04565;  //-0.0829
  axOffset =  -0.0742 + extraOffset; //potentially minus another 0.0829
  extraOffset = 0.3319; //with a bit - 0.0957
  ayOffset = 0.0653 + extraOffset ;
  float newAx = imu.calcAccel(imu.ax);
  float newAy = imu.calcAccel(imu.ay);
  axMean = average(axFive, newAx,axMean);
  ayMean = average(ayFive, newAy,ayMean);
  /*if(index >30){
    axResult = axMean;
    ayResult = ayMean;
  }else{
    axResult = newAx;
    ayResult = newAy;
  }*/
  
  Serial.print(axMean + axOffset,4);
  Serial.print(',');
  Serial.println(ayMean + ayOffset,4);
  //Serial.print(imu.calcAccel(imu.ax) + axOffset, 4);
  //Serial.print(", ");
  //Serial.println(imu.calcAccel(imu.ay) + ayOffset, 4);
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


float average(float arr[], float newValue, float preAverage){
  if(index<numAverage){
    float sum = 0;
    arr[index] = newValue;
    for(int i=0; i<=index;i++){
      sum = sum + arr[i];
    }
    return sum/(index+1);
  }else{
    float oldValue = arr[index%numAverage];
    arr[index%numAverage] = newValue;
    return preAverage + (newValue - oldValue)/numAverage;
  }
}

float headingCalc(float x, float y){
  //Serial.print(x);
  //Serial.print( "  ");
  //Serial.println(y);
  float h = 0.0;
   if(y ==0){
      if(x<0){
        h = 180.0;
       }else{
        h = 0.0;
    }
  }else{
    if(y>0){
      h = 90 - atan(x/y)*180/PI;
      //Serial.println("case 1");
    }else if(y<0){
      h = 270 - atan(x/y)*180/PI;
      //Serial.println("case 2");
    }
  }
  return h;
}

float manualHeadOffset(float h){
  if( (0<=h && h<80) || (350<h && h<=360)){
    h = h +10;
    if(h>360) h = h - 360;
  }else if( h>270 && h<=350){
    h = 9/8*h - 270/8;
  }else if(h >=80 && h<180){
    h = 9/10*h +18;
  }
  return h;
}


// Heading calculations taken from this app note:
// http://www51.honeywell.com/aero/common/documents/myaerospacecatalog-documents/Defense_Brochures-documents/Magnetic__Literature_Application_notes-documents/AN203_Compass_Heading_Using_Magnetometers.pdf
//mx has maximum about 2400 at east direction
//mx has minimum about 10 @ west direction
//mx amp = 1195; offset mx by -1205

//my has maximum about -3470 @ north direction
//my has minimum about -5800 @ 172 degrees S
// my amp = 1165; offset my by +4635


void printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  
  float heading;
  float mxOffset, myOffset;
  float resultHead;
  
  mxMean = average(mxFive, mx, mxMean);
  myMean = average(myFive, my, myMean);
  mxOffset = mxMean - 1205;
  myOffset = myMean + 4635;
  
  heading = headingCalc(mxOffset, myOffset);
  
  //headingMean = average(recentFive, heading, headingMean);
  if(heading<15 || heading>345){
    resultHead = heading;
  }else{
    resultHead = heading;
  }
  index++;
 // Serial.print("index: "); Serial.print(index);
  Serial.print('H'); Serial.print(resultHead, 3);
  //Serial.print("  average: "); Serial.println(headingMean,4);
}

int get_throttle(int t){
  if((t<1490 && t>1420) || t ==0){
    return 90;
  }else{
    if(t> 1490){
      return 108;
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

  //97,69 for run in a loop
  //86, 80 for running in line
  if((s<1500 && s>1440)|| s==0){
    return 83;
  }else{
    if(s>1500){
      return 97;
    }else{
      return 69;
    }
  }
}
