#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "U8glib.h"

const String systemCode = "System1"; 
int nextDisplay = 0;
long shift = 0;
long lastMsg = 0;
long lastSMS = 0;
const int alarmPin = 27;
bool Connected;
////////////////////////////////
#define dir_1 30
#define pwm_1 9
#define dir_2 31
#define pwm_2 10
const int speedi = 100;
const int speedi2 = 200;
const int saveDistance = 100;
const int trigPin1 = 36;
const int echoPin1 = 37;
const int trigPin2 = 38;
const int echoPin2 = 39;
const int trigPin3 = 40;
const int echoPin3 = 41;
const int trigPin4 = 42;
const int echoPin4 = 43;
bool movingAhead = false;
bool movingBack= false;
////////////////////////////////////////
bool isGas = false;
bool isFire = false;
bool isTemperature = false;
bool isbTemp = false;
RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

long firefighter = 0;
bool sendFire = false;
bool sendGas = false;
/////////////variables/////////////
double bTemp = 32.00;
int HBR = 80;
int temperature;
int humidity;

//////////////////////////////
int lightSwitch = 1;
int fanSwitch = 1;
int acSwitch = 1;
int doorSwitch = 1;
int help = 0;
////////////Screen/////////////
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);
bool tryit = true;
bool sent = true;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  setScreen();
  pinMode(pwm_1,OUTPUT);
  pinMode(dir_1,OUTPUT);
  pinMode(pwm_2,OUTPUT);
  pinMode(dir_2,OUTPUT);
  pinMode(trigPin1, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin1, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin3, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin3, INPUT); // Sets the echoPin as an Input
  pinMode(trigPin4, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin4, INPUT); // Sets the echoPin as an Input
  pinMode(alarmPin,OUTPUT);
  //pinMode(alarmPin,HIGH);
}
void loop() {
   //Serial.println("///////////////");
delay(1000);
  if(millis()>60000 && tryit)
  {
    Serial.println("**************************************");
    setupSMS();
    tryit = false;
  }
  if (radio.available()) {
    char text[30] = "";
    radio.read(&text, sizeof(text));
    char *data = text;
    //Serial.println(data);
    //Serial.println(text[5]);
    if(text[5] == 't')
    {
      bTemp = strtod(data,NULL);
      if((bTemp > 37 || bTemp < 30) && sent){
        //isbTemp = true;
        Display("Outer Body", "Temperature", "Exceeded Limit :(");
        sendSMS("Outer Body Temperature Exceeded Limit :");
        sendingData();
        sent = false;
      }
      else
      {
        sent = true;
        isbTemp = false;
      }
      Serial.println(bTemp);
    }
    else if(text[0] == 'm'){
      switch(text[1]){
        case '1':
          moveRight();
          Serial.println("RIGHT");
          break;
          
        case '2':
          moveBack();
          Serial.println("BACK");
          break;
        
        case '3':
          moveLeft();
          Serial.println("LEFT");
          break;
                    
        case '4':
          moveAhead();
          Serial.println("Ahead");
          break;

        case '5':
          moveAround();
          Serial.println("AROUND");
          break;

        case '6':
          Stop();
          Serial.println("STOP");
          break;
      } 
    }
    
    else if(text[0] == 'n'){
      switch(text[1]){
        case '1':
          lightSwitch = 1;
          Serial.println("LIGHT ON");
          sendingData();
          break;
          
        case '2':
          fanSwitch = 1;
          Serial.println("FAN ON");
          break;
        
        case '3':
          acSwitch = 1;
          Serial.println("AC ON");
          sendingData();
          break;
                    
        case '4':
          doorSwitch = 1;
          Serial.println("DOOR ON");
          sendingData();
          break;

        case '5':
          lightSwitch = 1;
          fanSwitch = 1;
          acSwitch = 1;
          doorSwitch = 1;
          Serial.println("ALL ON");
          sendingData();
          break;
      }
    }
    
    else if(text[0] == 'f'){
      switch(text[1]){
        case '1':
          lightSwitch = 0;
          Serial.println("LIGHT OFF");
          sendingData();
          break;
          
        case '2':
          fanSwitch = 0;
          Serial.println("FAN OFF");
          sendingData();
          break;
        
        case '3':
          acSwitch = 0;
          Serial.println("AC OFF");
          sendingData();
          break;
                    
        case '4':
          doorSwitch = 0;
          Serial.println("DOOR OFF");
          sendingData();
          break;

        case '5':
          lightSwitch = 0;
          fanSwitch = 0;
          acSwitch = 0;
          doorSwitch = 0;
          Serial.println("LOCK OFF");
          sendingData();
          break;
      }
    }

    else if(text[0] == 'l'){
      switch(text[1]){
        case '1':
          Display("Navigating from", "Toilet to", "Room");
          Serial3.write("g");
          delay(200);
          Serial.println("Navigating from Toilet to Room");
          break;
          
        case '2':
          Display("Navigating from", "Kitchen to", "Room");
          Serial3.write("b");
          Serial.println("Navigating from Kitchen to Room");
          break;
        
        case '3':
          Display("Navigating from", "Room to", "Toilet");
          Serial3.write("G");
          Serial.println("Navigating from Room to Toilet");
          break;
                    
        case '4':
          Display("Navigating from", "Kitchen to", "Toilet");
          Serial3.write("r");
          Serial.println("Navigating from Kitchen to Toilet");
          break;

        case '5':
          Display("Navigating from", "Room to", "Kitchen");
          Serial3.write("B");
          Serial.println("Navigating from Room to Kitchen");
          break;

        case '6':
          Display("Navigating from", "Toilet to", "Kitchen");
          Serial3.write("R");
          Serial.println("Navigating from Toilet to Kitchen");
          break;
      }
    }     
    else if(text[0] == 'h'){
      Serial.println("HELP ME");
      sendSMS("Your Patient Needs Your Help");
      sendingData();
      help = 1;
    }

    else if(text[0] == 'M'){
      Display("Movement voice", "commands is", "Active");
      Serial.println("Movement voice commands is Active");
    }
    else if(text[0] == 'N'){
      Display("Line Following", "feature is", "Active");
      Serial.println("Line Following feature is Active");
    }
    else if(text[0] == 'H'){
      Display("Head control", "feature is", "Active");
      Serial.println("Head control feature is Active");
    }
    else if(text[0] == 'O'){
      Display("Turn on", "appliances", "is Active");
      Serial.println("Turn on appliances is Active");
    }
    else if(text[0] == 'F'){
      Display("Switch off", "appliances", "is Active");
      Serial.println("Switch off appliances is Active");
    }
    else if(text[0] == 'T'){
      Display("Main group", "voice commands", "is Active");
      Serial.println("Main group voice commands is Active");
    }

    else{
      HBR = atoi(text);
      Serial.println(HBR);
    }
  }

  alarm();

  if(millis()-shift>10000 && millis()>10000)
  {
    displayNext();
  }

  setupSMS();
  if(millis()-lastMsg > 30000)
      sendingData();
  
  if(movingAhead){
    saveFront();
  }
  else if(movingBack){
    saveBack();
  }

  if(sendFire && millis() - firefighter > 300000){
    sendFire = false;
    sendSMS("Fire is Detected at MMU Multiple purpose Hall");
  }

  if(sendGas && millis() - firefighter > 300000){
    sendGas = false;
    sendSMS("Gas is Detected at MMU Multiple purpose Hall");
  }
}


void serialEvent1(){
  int value;
  if(Serial1.available())
  {
    /////////////////////
    if(movingAhead){
      saveFront();
    }
    else if(movingBack){
      saveBack();
    }
    ////////////////////
    //delay(100);
    Serial1.flush();
//    char *y = Serial1.read();
//    Serial.println(y);
    byte b1 = Serial1.read();
    byte b2 = Serial1.read();
    byte b3 = Serial1.read();
    int value = b1 * 256 + b2;
    //Serial.println(value);
    Serial.print((char)b1);
    Serial.println((char)b2);

    switch((char)b1){
      case 'm':
          switch((char)b2){
            case '1':
              moveRight();
              Serial.println("RIGHT");
              break;
              
            case '2':
              moveBack();
              Serial.println("BACK");
              break;
            
            case '3':
              moveLeft();
              Serial.println("LEFT");
              break;
                        
            case '4':
              moveAhead();
              Serial.println("AHEAD");
              break;
    
            case '5':
              moveAround();
              Serial.println("AROUND");
              break;
          }
          break;
      case 'f':
          switch((char)b2){
            case '1':
              Display("Light was", "Turned Off", "");
              Serial.println("LIGHT OFF");
              lightSwitch = 0;
              break;
              
            case '2':
              Display("Fan was", "Turned Off", "");
              Serial.println("FAN OFF");
              fanSwitch = 0;
              break;
            
            case '3':
              Display("AC was", "Turned Off", "");
              Serial.println("AC OFF");
              acSwitch = 0;
              break;
                        
            case '4':
              Display("Door was", "Closed", "");
              Serial.println("DOOR OFF");
              doorSwitch = 0;
              break;
    
            case '5':
              Display("All Devices", "Turned Off", "");
              Serial.println("All OFF");
              lightSwitch = 0;
              fanSwitch = 0;
              acSwitch = 0;
              doorSwitch = 0;
              break;
          }
          break;
      case 'n':
          switch((char)b2){
            case '1':
              Display("Light was", "Turned On", "");
              Serial.println("LIGHT ON");
              lightSwitch = 1;
              
              break;
              
            case '2':
              Display("Fan was", "Turned On", "");
              Serial.println("FAN ON");
              fanSwitch = 1;
              break;
            
            case '3':
              Display("AC was", "Turned On", "");
              acSwitch = 1;
              break;
                        
            case '4':
              Display("Door was", "Openned", "");
              Serial.println("DOOR ON");
              doorSwitch = 1;
              break;
    
            case '5':
              Display("All Devices", "Turned On", "");
              Serial.println("LOCK ON");
              lightSwitch = 1;
              fanSwitch = 1;
              acSwitch = 1;
              doorSwitch = 1;
              break;
          }
          break;
    case 's':
        Stop();
        Serial.println("STOP");
        break;

     case 'Y':
          switch((char)b2){
            case 'F':
              Display("Fire is", "Detected :(", "");
              sendSMS("Fire is Detected :(");
              isFire = true;
              Serial.println("Fire");
              Serial2.println("ATD+601114456443;"); // watch out here for semicolon at the end!!
              Serial.println("Calling  "); // print response over serial port 
              firefighter = millis(); 
              sendFire = true;            
              break;
              
            case 'G':
              Display("Gas is", "Detected :(", "");
              sendSMS("Gas is Detected :(");
              isGas = true;
              sendGas = true;
              firefighter = millis();
              break;
          }
          break;

    case 'N':
          switch((char)b2){
            case 'F':
              Display("Fire is", "not Detected :)", "");
              sendSMS("Fire is not Detected :)");
              Serial.println("No Fire");
              isFire = false;
              sendFire = false;
              break;
              
            case 'G':
              Display("Gas is", "Not Detected :)", "");
              sendSMS("Gas is not Detected :)");
              isGas = false;
              sendGas = false;
              Serial.println("No Gas");
              break;
          }
          break;

    case 'U':
          switch((char)b2){
            case 'C':
              Connected = false;
              Serial.println("Not Connected");
              break;
          }
          break;
          
    case 'C':
          switch((char)b2){
            case 'N':
              Connected = true;
              Serial.println("Connected");
              break;
          }
          break;
          
    case 'D':
          switch((char)b2){
            case 'Y':
              Display("Temperature", "Exceeded Limit", "");
              Serial.println("Temperature Exceeds limits");
              sendSMS("Surrounding Temperature Exceeds limits :(");
              isTemperature = true;
              break;

            case 'N':
              Display("Temperature is", "within Limit", "");
              sendSMS("Surrounding Temperature within limits :)");
              Serial.println("Temperature within limits");
              isTemperature = false;
              break;
          }
          break;      

    case 'H':
          value = b2 * 256 + b3;
          humidity = value;
          Serial.println("Humidity = ");
          Serial.println(value);
          break;

    case 'T':
          value = b2 * 256 + b3;
          temperature = value;
          Serial.print("Temp = ");
          Serial.println(value);
          break;
    }
  }
}

void serialEvent2(){
  String msg = "";
  while(Serial2.available())
  {
    char c =Serial2.read();
    msg.concat(c);
    delay(10);
  }
  Serial.println(msg);
  // Remove first 50 characters
  msg.remove(0, 50);
  int len = msg.length();
  // Remove \r\n from tail
  msg.remove(len - 2, 2);
  Serial.print("this is my msg");
  Serial.println(msg);
  if(msg.startsWith("\nm1")){
    moveRight();
    Serial.println("Received");
    Display("SMS Received:", "Moving Right","");
  }
  else if(msg.startsWith("\nm2")){
    moveBack();
    Serial.println("Received");
    Display("SMS Received:", "Moving Back","");
  }
  else if(msg.startsWith("\nm3")){
    moveLeft();
    Serial.println("Received");
    Display("SMS Received:", "Moving Left","");
  }
  else if(msg.startsWith("\nm4")){
    moveAhead();
    Serial.println("Received");
    Display("SMS Received:", "Moving Ahead","");
  }
  else if(msg.startsWith("\nm5")){
    moveAround();
    Serial.println("Received");
    Display("SMS Received:", "Moving Around","");
  }
  else if(msg.startsWith("\ns")){
    Stop();
    Serial.println("Received");
    Display("SMS Received:", "Stop","");
  }
}

void setScreen(){
    // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,0,0);
  }
  Display("", "Welcome Back:)", "");
         
}

void Display(String text1, String text2, String text3) {
  // Length (with one extra character for the null terminator)
  int str_len1 = text1.length() + 1; 
  int str_len2 = text2.length() + 1; 
  int str_len3 = text3.length() + 1; 
  
  // Prepare the character array (the buffer) 
  char text1_array[str_len1];
  char text2_array[str_len2];
  char text3_array[str_len3];
  // Copy it over 
  text1.toCharArray(text1_array, str_len1);
  text2.toCharArray(text2_array, str_len2);
  text3.toCharArray(text3_array, str_len3);
  
  // graphic commands to redraw the complete screen should be placed here  
  u8g.firstPage();  
  do {
      u8g.setFont(u8g_font_unifont);
      u8g.drawStr(10, 19, text1_array);
      u8g.drawStr(10, 38, text2_array);
      u8g.drawStr(10, 57, text3_array);
  } while( u8g.nextPage() );  
  shift = millis();          
}

void displayNext(){
  nextDisplay++;
  switch(nextDisplay){
    case 1: Display("Outer Body", "Temperature is:", (String)bTemp+" c");
            break;
    case 2: Display("Heart Beat", "Rate is:", (String)HBR+" bpm");
            break;
    case 3: Display("surrounding", "Temperature is:", (String)temperature+" c");
            break;
    case 4: Display("surrounding", "Humidity is:", (String)humidity+"%");
            break;
    case 5: if(Connected)
              Display("Wifi Status:", "Connected.", "");
            else
              Display("Wifi Status:", "not Connected.", "");
              nextDisplay = 0;
            break;
  }
}

void alarm(){
  if(isGas || isFire || isTemperature || isbTemp){
    Serial.println("alarm");
    digitalWrite(alarmPin, LOW);
  }
  else{
    Serial.println("no alarm");
    digitalWrite(alarmPin, HIGH);
  }
  
}

void sendingData(){
  if (Connected) {
    Serial1.write((int)bTemp / 256);
    Serial1.write((int)bTemp % 256);
    Serial1.write(HBR / 256);
    Serial1.write(HBR % 256);
    Serial1.write(lightSwitch / 256);
    Serial1.write(lightSwitch % 256);
    Serial1.write(fanSwitch / 256);
    Serial1.write(fanSwitch % 256);
    Serial1.write(acSwitch / 256);
    Serial1.write(acSwitch % 256);
    Serial1.write(doorSwitch / 256);
    Serial1.write(doorSwitch % 256);
    Serial1.write(help / 256);
    Serial1.write(help % 256);
    Serial.print("Heartbeat Rate: ");
    Serial.println(HBR);
    help = 0;
  }
  lastMsg = millis();
}

void setupSMS(){
  if(!Connected && millis()-lastSMS > 60000)
  {
    String sms= "Outer Body Temperature is:"+(String)bTemp+" c \n Heart Beat Rate is:"+(String)HBR+" bpm \n";
           sms+= "surrounding Temperature is:"+(String)temperature+" c \n surrounding Humidity is:"+(String)humidity+"% \n";
           sms+= "Wifi Status not Connected.";
    sendSMS(sms); 
    lastSMS = millis();
  }
}

void sendSMS(String message){

  Serial2.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  Serial2.println("AT+CMGS=\"+601114456443\"\r");
  /////////////////////
    if(movingAhead){
      saveFront();
    }
    else if(movingBack){
      saveBack();
    }
    ////////////////////
  delay(1000);
  Serial.println(message);
  Serial2.println(message);// The SMS text you want to send // "sys"+systemCode+": "+
  delay(100);
  /////////////////////
    if(movingAhead){
      saveFront();
    }
    else if(movingBack){
      saveBack();
    }
  ////////////////////
  Serial2.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void moveAhead(){
  digitalWrite(dir_1,HIGH);
  analogWrite(pwm_1, speedi);
  digitalWrite(dir_2,HIGH);
  analogWrite(pwm_2, speedi);
  movingAhead = true;
  movingBack = false;
  saveFront();
  delay(1500);
  digitalWrite(dir_1,HIGH);
  analogWrite(pwm_1, 50);
  digitalWrite(dir_2,HIGH);
  analogWrite(pwm_2, 50);
  //Stop();
}

void moveBack(){
  digitalWrite(dir_1,LOW);
  analogWrite(pwm_1, speedi);
  digitalWrite(dir_2,LOW);
  analogWrite(pwm_2, speedi);
  movingBack = true;
  movingAhead = false;
  saveBack();
  delay(1500);
  digitalWrite(dir_1,LOW);
  analogWrite(pwm_1, 50);
  digitalWrite(dir_2,LOW);
  analogWrite(pwm_2, 50);
  //Stop();
  
}

void moveRight(){
  digitalWrite(dir_1,LOW);
  analogWrite(pwm_1, speedi2);
  digitalWrite(dir_2,HIGH);
  analogWrite(pwm_2, speedi2);
  //analogWrite(pwm_2, 0);
  delay(750);
  Stop();
}

void moveLeft(){
  digitalWrite(dir_2,LOW);
  analogWrite(pwm_2, speedi2);
  digitalWrite(dir_1,HIGH);
  analogWrite(pwm_1, speedi2);
  //analogWrite(pwm_1, 0);
  delay(750);
  Stop();
}

void moveAround(){
  digitalWrite(dir_1,HIGH);
  analogWrite(pwm_1, speedi2);
  digitalWrite(dir_2,LOW);
  analogWrite(pwm_2, speedi2);
  delay(1500);
  Stop();
}

void Stop(){
  analogWrite(pwm_1, 0);
  analogWrite(pwm_2, 0);
  movingAhead = false;
  movingBack = false;
}

void saveFront(){
  long duration1;
  long duration2;
  int distance1;
  int distance2;
  
  // Clears the trigPin
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin1, HIGH);
  // Calculating the distance
  distance1 = duration1*0.034/2;

    // Clears the trigPin
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);
  // Calculating the distance
  distance2 = duration2*0.034/2;
  if(distance1 < saveDistance || distance2 < saveDistance)
  {    
    digitalWrite(dir_1,HIGH);
    analogWrite(pwm_1, 200);
    digitalWrite(dir_2,HIGH);
    analogWrite(pwm_2, 200);
    delay(100);
    Stop();
    Display("An Obstacle", "is Detected", "in Front");
    Serial.print("stop: ");
    Serial.println(distance1);
    Serial.println(distance2);
  }
  Serial.print("Distance: ");
  Serial.println(distance1);
}

void saveBack(){
  long duration1;
  long duration2;
  int distance1;
  int distance2;
  
  // Clears the trigPin
  digitalWrite(trigPin3, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin3, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoPin3, HIGH);
  // Calculating the distance
  distance1 = duration1*0.034/2;

  // Clears the trigPin
  digitalWrite(trigPin4, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin4, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin4, HIGH);
  // Calculating the distance
  distance2 = duration2*0.034/2;
  if(distance1 < saveDistance || distance2 < saveDistance)
  {
    digitalWrite(dir_1,HIGH);
    analogWrite(pwm_1, 200);
    digitalWrite(dir_2,HIGH);
    analogWrite(pwm_2, 200);
    delay(100);
    Stop();
    Display("An Obstacle", "is Detected", "at the Back");
    Serial.print("stop: ");
    Serial.println(distance1);
    Serial.println(distance2);
  }
  Serial.print("Distance: ");
  Serial.println(distance1);
}
