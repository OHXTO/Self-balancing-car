#include<PID_v2.h>
#include<stdlib.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

int Left_PWM_PIN = 12;          int Right_PWM_PIN = 13; 
int Left_Forward_PIN = 10;      int Right_Forward_PIN = 9;
int Left_Reverse_PIN = 11;      int Right_Reverse_PIN = 7;
int Gyropin = A3;
int Accelpin = A4;
int Magpin = A5;

double Kp = 53.2;//33.2; 53.2
double Ki = 244;//244;
double Kd = 5.127;//0.033; 5.127

Adafruit_BNO055 bno = Adafruit_BNO055(-1, 0x28);

double gravX;
double PWMvalue = 0.0;
double gxSet = -0.15;

PID myPID(&gravX,&PWMvalue,&gxSet,Kp, Ki, Kd, DIRECT); // (&gravX, &PWMvalue, &gxSet, kp, ki, kd)


  //Function: tell direction and apply pwm to wheel
void applyPWM(double pwmValue,int PWMPIN, int dirPIN1, int dirPIN2){
    
   if (pwmValue >= 0) {
    digitalWrite(dirPIN1, HIGH);
    digitalWrite(dirPIN2, LOW);
    analogWrite(PWMPIN,abs(pwmValue));
    } 
    else {
    digitalWrite(dirPIN1, LOW);
    digitalWrite(dirPIN2, HIGH);
    analogWrite(PWMPIN,abs(pwmValue));
    }
    
  }

void Check_leds(uint8_t gyro,uint8_t accel,uint8_t mag){
  if (gyro==3){
    digitalWrite(Gyropin, HIGH);
    }
  if (accel==3){
    digitalWrite(Accelpin, HIGH);
    }
  if (mag==3){
    digitalWrite(Magpin, HIGH);
    }
  
  }
  
void setup(){ 
  Serial.begin(115200); 
  pinMode(Left_PWM_PIN, OUTPUT);             pinMode(Right_PWM_PIN, OUTPUT);
  pinMode(Left_Forward_PIN, OUTPUT);         pinMode(Right_Forward_PIN, OUTPUT);
  pinMode(Left_Reverse_PIN, OUTPUT);         pinMode(Right_Reverse_PIN, OUTPUT);
  pinMode(Gyropin, OUTPUT);   
  pinMode(Accelpin, OUTPUT);
  pinMode(Magpin, OUTPUT);

  //test if bno work
  if (!bno.begin()) {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }

  //Read Temperature
  int8_t temp = bno.getTemp();
  Serial.print("Current Temperature: ");
  Serial.print(temp);
  Serial.println(" C");
  Serial.println("");
  
  bno.setExtCrystalUse(true);
  // Initialize PID
  myPID.SetOutputLimits(-255, 255);
  myPID.SetMode(AUTOMATIC);
  }


void loop(){ 
  imu::Vector<3> grav = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  
  Serial.println((String)millis() + ", " + gravX);
  Serial.println(PWMvalue);
  uint8_t system, gyro, accel, mag = 0;

  gravX = -grav.x();
  myPID.Compute();

  
  applyPWM(PWMvalue,Left_PWM_PIN , Left_Forward_PIN , Left_Reverse_PIN );
  applyPWM(PWMvalue,Right_PWM_PIN, Right_Forward_PIN, Right_Reverse_PIN);
  
      
      bno.getCalibration(&system, &gyro, &accel, &mag);
      Check_leds(gyro,accel,mag);
      
      Serial.print("CALIBRATION: Sys=");
      Serial.print(system, DEC);
      Serial.print(" Gyro=");
      Serial.print(gyro, DEC);
      Serial.print(" Accel=");
      Serial.print(accel, DEC);
      Serial.print(" Mag=");
      Serial.println(mag, DEC);
  
  delay(50); 



} 
