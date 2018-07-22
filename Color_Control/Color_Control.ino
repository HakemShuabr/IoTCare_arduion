// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial (2, 3);

//int yellow = 7;
//int red = 6;

#define dir_1 12
#define pwm_1 9
#define dir_2 13
#define pwm_2 10
const int speedi = 100;
const int speedi2 = 200;

bool turnLeftR;
bool turnLeftL;
long firstPush = 0;
bool oneTime = true;

extern "C" {
#include "utility/twi.h" // from Wire library, so we can do bus scanning
}
#include "Adafruit_TCS34725.h"
bool outOfLineR = false;
bool outOfLineL = false;
bool wasOutRight = false;
bool wasOutLeft = false;
char color = 'g';
Adafruit_TCS34725 tcs1 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
Adafruit_TCS34725 tcs2 = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define TCAADDR 0x70

bool roomTotoilet = false;
bool ToiletToroom = false;

void tcaselect(uint8_t i) {
if (i > 7) return;
Wire.beginTransmission(TCAADDR);
Wire.write(1 << i);
Wire.endTransmission();
}

AF_DCMotor motor1(2);
AF_DCMotor motor2(3);
void setup() {
  Serial.begin(9600); 
  mySerial.begin(9600);
  // set up Serial library at 9600 bps
  Serial.println("Motor test!");
  Wire.begin();
//  pinMode(yellow, OUTPUT);
//  pinMode(red, OUTPUT);

//  digitalWrite(yellow, LOW);
//  digitalWrite(red, LOW);
  // turn on motor
  motor1.setSpeed(150);
  motor2.setSpeed(150);
  tcaselect(4);
  if (tcs1.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  tcaselect(7);
   if (tcs2.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  pinMode(pwm_1,OUTPUT);
  pinMode(dir_1,OUTPUT);
  pinMode(pwm_2,OUTPUT);
  pinMode(dir_2,OUTPUT);
  digitalWrite(dir_1,HIGH);
}

void loop() {
    
  rightSide(color);
  leftSide(color);
  if(outOfLineR && outOfLineL){
    motor1.run(RELEASE);
    motor2.run(RELEASE);
    analogWrite(pwm_1, 0);
    analogWrite(pwm_2, 0);
    firstPush = 0;
    Serial.println("Stop");
  }
  else if(outOfLineR){
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    digitalWrite(dir_2,HIGH);
    analogWrite(pwm_2, 100);
    Serial.println("LEFT");
    wasOutRight = true;
    firstPush = 0;

  }
  else if(outOfLineL){
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    digitalWrite(dir_1,HIGH);
    analogWrite(pwm_1, 100); 
    Serial.println("RIGHT");
    wasOutLeft = true;
    firstPush = 0;
  }
  else if(turnLeftR || turnLeftL){
    if(oneTime){
      afterTurn();
      oneTime = false;
    }
    else{
      digitalWrite(dir_1,HIGH);
      digitalWrite(dir_2,HIGH);
      analogWrite(pwm_1, 100);
      analogWrite(pwm_2, 100);
      delay(200);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
  }
  else{
    if(wasOutRight){
      Serial.print("RRRRRRRRRRRRRRRRRRRRRRRRRR");
      wasOutRight = false;
      digitalWrite(pwm_1, HIGH);
      analogWrite(pwm_1, 100);
      analogWrite(pwm_2, 0);
      motor1.run(BACKWARD);
      motor2.run(FORWARD);
      delay(1000);
      motor1.run(RELEASE);
      motor2.run(RELEASE);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
    else if(wasOutLeft){
      Serial.print("LLLLLLLLLLLLLLLLLLLLLLLLLLL");
      wasOutLeft = false;
      digitalWrite(dir_2,HIGH);
      analogWrite(pwm_2, 100);
      analogWrite(pwm_1, 0);
      delay(1000);
      motor1.run(RELEASE);
      motor2.run(RELEASE);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
    else{
      firstPush++;
      motor1.run(FORWARD);
      motor2.run(FORWARD);
      digitalWrite(dir_1,HIGH);
      digitalWrite(dir_2,HIGH);
      if(firstPush < 3){
        analogWrite(pwm_1, 100);
        analogWrite(pwm_2, 100);
      }
      else{
        analogWrite(pwm_1, 50);
        analogWrite(pwm_2, 50);
      }
      delay(500);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);   
      Serial.println("GOGO");
    }
  }
}

void getColor(int colorSensor, float& r,float& g, float& b){
  tcaselect(colorSensor);
  uint8_t i;
  uint16_t clear, red, green, blue;
  
  tcs1.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs1.getRawData(&red, &green, &blue, &clear);

  tcs1.setInterrupt(true);  // turn off LED
    
  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
//  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(r);
  Serial.print("\tG:\t"); Serial.print(g);
  Serial.print("\tB:\t"); Serial.println(b);
}

void rightSide(char lineColor){
  tcaselect(4);
  uint8_t i;
  uint16_t clear, red, green, blue;
  
  tcs1.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs1.getRawData(&red, &green, &blue, &clear);

  tcs1.setInterrupt(true);  // turn off LED
    
  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(r);
  Serial.print("\tG:\t"); Serial.print(g);
  Serial.print("\tB:\t"); Serial.println(b);
  switch(lineColor){
    case 'r':
      if(r > 100 && b < 100 && g < 100)
        outOfLineR = false;
      else
        outOfLineR = true;
      break;
      
    case 'b':
      if(r < 100 && b > 100 && g < 100)
        outOfLineR = false;
      else
        outOfLineR = true;
      break;
      
    case 'g':
      if(r < 100 && b < 100 && g > 100)
        outOfLineR = false;
      else if(r > 100 && b < 100 && g < 100){
        turnLeftR = true;
        outOfLineR = false;
        outOfLineL = false;
      }
      else
        outOfLineR = true;
      break;     
  }
}

void leftSide(char lineColor){
  tcaselect(7);
  uint8_t i;
  uint16_t clear, red, green, blue;
  
  tcs2.setInterrupt(false);      // turn on LED

  delay(60);  // takes 50ms to read 
  
  tcs2.getRawData(&red, &green, &blue, &clear);

  tcs2.setInterrupt(true);  // turn off LED
    
  // Figure out some basic hex code for visualization
  uint32_t sum = clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;

  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR2:\t"); Serial.print(r);
  Serial.print("\tG2:\t"); Serial.print(g);
  Serial.print("\tB2:\t"); Serial.println(b);
  switch(lineColor){
    case 'r':
      if(r > 100 && b < 100 && g < 100)
        outOfLineL = false;
      else
        outOfLineL = true;
      break;
      
    case 'b':
      if(r < 100 && b > 100 && g < 100)
        outOfLineL = false;
      else
        outOfLineL = true;
      break;
      
    case 'g':
      if(r < 100 && b < 100 && g > 100)
        outOfLineL = false;
      else if(r > 100 && b < 100 && g < 100){
        turnLeftL = true;
        outOfLineL = false;
        outOfLineR = false;
      }
      else
        outOfLineL = true;
      break;     
  }
}

void afterTurn(){
  float r, g, b;
  
  digitalWrite(dir_1,HIGH);
  analogWrite(pwm_1, 200);
  digitalWrite(dir_2,LOW);
  analogWrite(pwm_2, 200); 
  delay(800);
  analogWrite(pwm_1, 0);
  analogWrite(pwm_2, 0);

  bool oneTime1 = true;
  while(true){
    turnLeftR = true;
    turnLeftL = true;
    digitalWrite(dir_1,HIGH);
    analogWrite(pwm_1, 100);
    digitalWrite(dir_2,HIGH);
    analogWrite(pwm_2, 100);
    delay(200);
    analogWrite(pwm_1, 0);
    analogWrite(pwm_2, 0);
    getColor(4, r, g, b);
    if(r > 100 && b < 100 && g < 100)
      turnLeftR=false;
    else if(r < 100 && b < 100 && g > 100)
      break;
    getColor(7, r, g, b);
    if(r > 100 && b < 100 && g < 100)
      turnLeftL = false;
    else if(r < 100 && b < 100 && g > 100)
      break;
    if(turnLeftR && turnLeftL && oneTime1){
      while(true){
        digitalWrite(dir_1,LOW);
        analogWrite(pwm_1, 100);
        digitalWrite(dir_2,LOW);
        analogWrite(pwm_2, 100); 
        delay(250);
        analogWrite(pwm_1, 0);
        analogWrite(pwm_2, 0);
        getColor(4, r, g, b);
        if(r > 100 && b < 100 && g < 100)
          break;
        getColor(7, r, g, b);
        if(r > 100 && b < 100 && g < 100)
          break;
      }
      digitalWrite(dir_1,HIGH);
      analogWrite(pwm_1, 200);
      digitalWrite(dir_2,LOW);
      analogWrite(pwm_2, 200); 
      delay(300);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
      oneTime1 = false;
    }
    else if(turnLeftR && !turnLeftL){
      analogWrite(pwm_1, 0);
      digitalWrite(dir_2,HIGH);
      analogWrite(pwm_2, 200); 
      delay(250);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
    if(turnLeftL && !turnLeftR){
      digitalWrite(dir_1,HIGH);
      analogWrite(pwm_1, 200);
      analogWrite(pwm_2, 0); 
      delay(250);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
  }
}

void beforeTurn(){
  float r, g, b;
  digitalWrite(dir_1,LOW);
  analogWrite(pwm_1, 200);
  digitalWrite(dir_2,HIGH);
  analogWrite(pwm_2, 200); 
  delay(800);
  analogWrite(pwm_1, 0);
  analogWrite(pwm_2, 0);
  bool oneTime1 = true;
  while(true){
    turnLeftR = true;
    turnLeftL = true;
    digitalWrite(dir_1,HIGH);
    analogWrite(pwm_1, 100);
    digitalWrite(dir_2,HIGH);
    analogWrite(pwm_2, 100);
    delay(200);
    analogWrite(pwm_1, 0);
    analogWrite(pwm_2, 0);
    getColor(4, r, g, b);
    if(r > 100 && b < 100 && g < 100)
      turnLeftR=false;
    else if(r < 100 && b < 100 && g > 100)
      break;
    getColor(7, r, g, b);
    if(r > 100 && b < 100 && g < 100)
      turnLeftL = false;
    else if(r < 100 && b < 100 && g > 100)
      break;
    if(turnLeftR && turnLeftL && oneTime1){
      while(true){
        digitalWrite(dir_1,LOW);
        analogWrite(pwm_1, 100);
        digitalWrite(dir_2,LOW);
        analogWrite(pwm_2, 100); 
        delay(250);
        analogWrite(pwm_1, 0);
        analogWrite(pwm_2, 0);
        getColor(4, r, g, b);
        if(r > 100 && b < 100 && g < 100)
          break;
        getColor(7, r, g, b);
        if(r > 100 && b < 100 && g < 100)
          break;
      }
      digitalWrite(dir_1,LOW);
      analogWrite(pwm_1, 200);
      digitalWrite(dir_2,HIGH);
      analogWrite(pwm_2, 200); 
      delay(300);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
      oneTime1 = false;
    }
    else if(turnLeftR && !turnLeftL){
      analogWrite(pwm_1, 0);
      digitalWrite(dir_2,HIGH);
      analogWrite(pwm_2, 200); 
      delay(250);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
    if(turnLeftL && !turnLeftR){
      digitalWrite(dir_1,HIGH);
      analogWrite(pwm_1, 200);
      analogWrite(pwm_2, 0); 
      delay(250);
      analogWrite(pwm_1, 0);
      analogWrite(pwm_2, 0);
    }
  }
}

