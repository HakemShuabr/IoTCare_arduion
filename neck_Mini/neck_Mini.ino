//simple Tx on pin D12
//Written By : Mohannad Rawashdeh
// 3:00pm , 13/6/2013
//http://www.genotronex.com/
//..................................
#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <nRF24L01.h>
#include <ADXL345.h>
#include <RF24.h>

/////////////////////////////////////
VR myVR(2,3);    // 2:RX 3:TX, you can choose your favourite pins.

bool ONorOFF;  // to toggle between turning on and off the appliances
bool headControl = false; // to enable the head control of the wheelchair
uint8_t buf[64]; // buffer to read the voice recognization 

String destination = ""; // define the destination of the line follower example: room, kitchen
int dir= -1;
#define Move      (0) // 21 verables of for 21 commands
#define navigate  (1) 
#define gesture   (2)
#define terminate (3)
#define right     (4)
#define back      (5)
#define left      (6)
#define front     (7)
#define around    (8)
#define Stop      (9)
#define room      (10)
#define toilet    (11)
#define kitchen   (12)
#define turnOn    (13)
#define switchOff (14)
#define light     (15)
#define fan       (16)
#define ac        (17)
#define lock      (18)
#define all       (19)
#define help      (20)
//////////////////////////////////////

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

//////////////////////////////////////

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "00001";

const int buzzer = 4;
const int blueLED = 6;
int inputPulse = A1;
int pulse=0;
char pulseChar[4];
char *command;
char *data;
///////////////////////////
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(5);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
double temperature = 0;
int roomTemp = 0;
char tempChar[10];
///////////////////////////////////

long lastUpdate = 0;

int greenPin = 2;
int bluePin = 2;

/////////////////////////////////////////////////////////
int BPM=0;
int beats = 0;

long minute = 0;

#define threshold 550 // define threshold for heart beat
//////////////////////////////////////////////////////////
void setup() {
  myVR.begin(9600); // start the voice recognization module
  Serial.begin(9600);
  pinMode(inputPulse, INPUT);
  sensors.begin();
  ///////////////////////////
  adxl.powerOn();           // starting the 3-axis accelerometer 
  //////////////////////////  
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  ////////////////////////////
 
  pinMode(buzzer, OUTPUT);
  pinMode(blueLED, OUTPUT);
  tone(buzzer, 2048, 200);
  digitalWrite(blueLED, HIGH);
  delay(500);
  digitalWrite(blueLED, LOW);
  
  if(myVR.clear() == 0){ // clear the recognizer from recordes
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  loadMainRecord();
}

void loop(){
  
  voiceRecognize();

  if(headControl)
    headPointer();
  
  // Send the command to get temperatures
  sensors.requestTemperatures();  
  temperature = sensors.getTempCByIndex(0); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  Serial.print("Temperature is: ");
  Serial.println(temperature); 

  ///////////////////////////////////////  
  pulse = analogRead(inputPulse);

  if(pulse<=threshold)
  {
    beats++;
  }

  // to send heartbeat every minute
  long now = millis();
  if (now - minute > 60000) {
    minute = now;
    BPM = 2*beats;
    beats = 0;
    sprintf (pulseChar, "%1d", BPM);
    radio.write(&pulseChar, sizeof(pulseChar));
    delay(500);
    Serial.print("BPM : ");
    Serial.println(BPM);
  }

  now = millis();
  // to send temperature every minute
  if (now - lastUpdate > 20000) {
    digitalWrite(blueLED, HIGH);
    lastUpdate = now;
    if(beats == 0 && now - minute > 15000){
      BPM = beats;
      sprintf (pulseChar, "%1d", BPM);
      radio.write(&pulseChar, sizeof(pulseChar));
      delay(500);
    }
    
    dtostrf(temperature, 4, 2, tempChar);
    tempChar[5]='t';
    radio.write(&tempChar, sizeof(tempChar));
    delay(500);
    digitalWrite(blueLED, LOW);
  }
  
  Serial.print("value is: ");
  Serial.println(pulse);
  Serial.print("pulse is: ");
  Serial.println(beats);

  //sprintf (pulseChar, "%1d", pulse);
  //radio.write(&pulseChar, sizeof(pulseChar));

   //serial.print("*************************************");
  delay(200);
}

/*
 * Function:  sendcommand 
 * --------------------
 * sends message or command to the main circuit through radio frequency 
 *
 *  command: command to be send
 *  Size: the size of the command to be send
 *  
 */

void sendcommand(char *command, int Size){
  radio.write(command, Size);
  delay(500); 
}
/*
 * Function:  loadMainRecord 
 * --------------------
 * load the main records (commands) to be used 
 * 
 */

void loadMainRecord(){
  myVR.clear();
  myVR.load((uint8_t)Move);
  myVR.load((uint8_t)navigate);
  myVR.load((uint8_t)gesture);
  myVR.load((uint8_t)terminate);
  myVR.load((uint8_t)turnOn);
  myVR.load((uint8_t)switchOff);
  myVR.load((uint8_t)help);
}
/*
 * Function:  loadNavigationRecord
 * --------------------
 * load the records (commands) which will be used to navigate between rooms
 * example: room, kithchen and toilet
 * 
 */
void loadNavigationRecord(){
  myVR.clear();
  myVR.load((uint8_t)room);
  myVR.load((uint8_t)toilet);
  myVR.load((uint8_t)kitchen);
  myVR.load((uint8_t)terminate);
}
/*
 * Function:  loadMoveRecord
 * --------------------
 * load the records (commands) which will be used to
 * move the wheelchair in different directions
 * example: right, left, front, back ...ect
 * 
 */
void loadMoveRecord(){
  myVR.clear();
  myVR.load((uint8_t)right);
  myVR.load((uint8_t)back);
  myVR.load((uint8_t)left);
  myVR.load((uint8_t)front);
  myVR.load((uint8_t)around);
  myVR.load((uint8_t)Stop);
  myVR.load((uint8_t)terminate);
}
/*
 * Function:  loadMoveRecord
 * --------------------
 * load the records (commands) which will be used to
 * toggle room appliances example: turn on the light.
 * 
 */
void loadApplianceRecord(){
  myVR.clear();
  myVR.load((uint8_t)light);
  myVR.load((uint8_t)fan);
  myVR.load((uint8_t)ac);
  myVR.load((uint8_t)lock);
  myVR.load((uint8_t)all);
  myVR.load((uint8_t)terminate);
}
/*
 * Function:  voiceRecognize
 * --------------------
 * read the available commands from the voice recognization module
 * and perform the required function based on the voice command.
 * 
 */
void voiceRecognize(){
  int cmd = myVR.recognize(buf, 50);
  if(cmd>0){
    tone(buzzer, 2048, 200);
    digitalWrite(blueLED, HIGH);
    delay(500);
    digitalWrite(blueLED, LOW);
    Serial.print("yeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeees");
    switch(buf[1]){
      case Move:
        /** turn on LED */
        loadMoveRecord();
        sendcommand("M", 1);
        Serial.println("Movement");
        break;
      case navigate:
        loadNavigationRecord();
        sendcommand("N", 1);
        Serial.println("Navigate");
        break;
      case gesture:
        headControl = true;
        sendcommand("H", 1);
        Serial.println("Gesture");
        break;
      case terminate:
        Serial.println("Terminate");
        sendcommand("T", 1);
        destination = "";
        headControl = false;
        loadMainRecord();
        break;
      case right:
        sendcommand("m1", 2);
        Serial.println("Right");
        break;
      case back:
        sendcommand("m2", 2);
        Serial.println("Back");
        break;
      case left:
        sendcommand("m3", 2);
        Serial.println("Left");
        break;
      case front:
        sendcommand("m4", 2);
        Serial.println("Front");
        break;
      case around:
        sendcommand("m5", 2);
        Serial.println("Around");
        break;
      case Stop:
        sendcommand("m6", 2);
        Serial.println("Stop");
        break;
      case room:
        if(destination == "toilet"){
          sendcommand("l1", 2);
          destination = "";
        }
        else if(destination == "kitchen"){
          sendcommand("l2", 2);
          destination = "";
        }
        else 
          destination = "room";
        Serial.println("Room");
        break;
      case toilet:
        if(destination == "room"){
          sendcommand("l3", 2);
          destination = "";
        }
        else if(destination == "kitchen"){
          sendcommand("l4", 2);
          destination = "";
        }
        else 
          destination = "toilet";
        Serial.println("Toilet");
        break;
      case kitchen:
        if(destination == "room"){
          sendcommand("l5", 2);
          destination = "";
        }
        else if(destination == "toilet"){
          sendcommand("l2", 2);
          destination = "";
        }
        else 
          destination = "kitchen";
        Serial.println("Kitchen");
        break;
      case turnOn:
        /** turn off LED*/
        ONorOFF = true;
        sendcommand("O", 1);
        loadApplianceRecord();
        Serial.println("Turn On");
        break;
      case switchOff:
        /** turn off LED*/
        ONorOFF = false;
        sendcommand("F", 1);
        loadApplianceRecord();
        Serial.println("Switch Off");
        break;
      case light:
        if(ONorOFF)
          sendcommand("n1", 2);
        else
          sendcommand("f1", 2);
        delay(100);
        Serial.println("Light");
        break;
      case fan:
        if(ONorOFF)
          sendcommand("n2", 2);
        else
          sendcommand("f2", 2);
        delay(100);
        Serial.println("Fan");
        break;
      case ac:
        if(ONorOFF)
          sendcommand("n3", 2);
        else
          sendcommand("f3", 2);
        delay(100);
        Serial.println("AC");
        break;
      case lock:
        if(ONorOFF)
          sendcommand("n4", 2);
        else
          sendcommand("f4", 2);
        delay(100);
        Serial.println("Lock");
        break;
      case all:
        if(ONorOFF)
          sendcommand("n5", 2);
        else
          sendcommand("f5", 2);
        delay(100);
        Serial.println("All");
        break;
      case help:
        sendcommand("h", 1);
        Serial.println("Help");
        delay(100);
        break;
      default:
        Serial.println("Record function undefined");
        break;
    }
    /** voice recognized */
  }
}

/*
 * Function:  headGesture
 * --------------------
 * detects head gesture of the head with the help of 3-axis accelerometer
 * and sends commands of movement direction
 * 
 */
void headPointer(){
  
  double xyz[3];
  adxl.getAcceleration(xyz); // read the x y z values
  if(xyz[0] > 0.5){
    if(dir != 1){
      dir= 1;
      sendcommand("m1", 2);
      Serial.println("Move Right");
      digitalWrite(2, HIGH);
    }
  }
  else if(xyz[0] < -0.5){
    if(dir != 3){
      dir= 3;
      sendcommand("m3", 2);
      Serial.println("Move Lift");
      digitalWrite(3, HIGH);
    }
  }
  else if(xyz[2] < -0.5){
    if(dir != 4){
      dir= 4;
      sendcommand("m4", 2);
      Serial.println("Move Ahead");
      digitalWrite(2, HIGH);
      digitalWrite(3, HIGH);
    }
  }
  else if(xyz[2] > 0.5){
    if(dir != 2){
      dir= 2;
      sendcommand("m2", 2);
      Serial.println("Move Back");
    }
  }
  else if(dir != -1){
    dir = -1;
    sendcommand("m6", 2);
    Serial.println("Stop");
    digitalWrite(3, LOW);
    digitalWrite(2, LOW);
  }
}

