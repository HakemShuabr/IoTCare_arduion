#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
//////////////////////////////////////
int HBR = 80;
double bTemp = 34.0;
int help = 0;
int lightSwitch = 0;
int fanSwitch = 0;
int acSwitch = 0;
int door = 0;
int lock = 0;
long lastMsg = 0;
long tryNow = 0;
bool Connected;
//////////////////////////////////////
#define DHTPIN 14     // what pin we're connected to
#define DHTTYPE DHT22
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
int humidity = 0;
int temperature = 0;
//////////////////////////////////////
const int firePin = 12;
int fireReading = 1;
///////////////////////////////////
const int alarmPin = 5;
const int redLED = 4;
const int blueLED = 4;
bool activeAlarm;
///////////////////////////////////
int gas = 1;
const int gasPin=13;  
//long LPG = 0;
//long CO = 0;
//long smoke = 0;
//const int gasPin=A0;                                
//int RL_VALUE=5;                                     //define the load resistance on the board, in kilo ohms
//double RO_CLEAN_AIR_FACTOR=9.83;                     //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
//                                                     //which is derived from the chart in datasheet
//
//int CALIBARAION_SAMPLE_TIMES=50;                    //define how many samples you are going to take in the calibration phase
//int CALIBRATION_SAMPLE_INTERVAL=500;                //define the time interal(in milisecond) between each samples in the
//                                                    //cablibration phase
//int READ_SAMPLE_INTERVAL=50;                        //define how many samples you are going to take in normal operation
//int READ_SAMPLE_TIMES=5;                            //define the time interal(in milisecond) between each samples in 
//                                                    //normal operation
//#define         GAS_LPG             0   
//#define         GAS_CO              1   
//#define         GAS_SMOKE           2     
//
//double           LPGCurve[3]  =  {2.3,0.21,-0.47};   //two points are taken from the curve. 
//                                                    //with these two points, a line is formed which is "approximately equivalent"
//                                                    //to the original curve. 
//                                                    //data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59) 
//double           COCurve[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve. 
//                                                    //with these two points, a line is formed which is "approximately equivalent" 
//                                                    //to the original curve.
//                                                    //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
//double           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve. 
//                                                    //with these two points, a line is formed which is "approximately equivalent" 
//                                                    //to the original curve.
//                                                    //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
//double           Ro           =  10;                 //Ro is initialized to 10 kilo ohms
///////////////////////////////////////////////////////
// Update these with values suitable for your network.

const char* ssid = "Lnet";  //sbahasan@unifi
const char* password = "87654321";//tdhv5638";  //0183887285
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastUpdate = 0;
char msg[50];


void setup_wifi(){

  delay(10);
   //We start by connecting to a WiFi network
  /*Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);*/

  WiFi.begin(ssid, password);
  tryNow = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - tryNow < 15000)) {
    digitalWrite(blueLED, HIGH);
    delay(500);
    /*Serial.print(".");*/
    digitalWrite(blueLED, LOW);
    fireDetection();
    gasDetection();
  }
  if(WiFi.status() == WL_CONNECTED){
    Connected = true;
    digitalWrite(blueLED, HIGH);
  }
  else{
    Connected = false;
    digitalWrite(blueLED, LOW);
  }
  
  randomSeed(micros());

  /*Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());*/
}

void callback(char* topic, byte* payload, unsigned int length) {
    for(int i = 0; i < length; i++){
       //Serial.println((char)payload[i]);
       //Serial.println(i);
       Serial.write(payload[i]);
       
    }
     digitalWrite(redLED, HIGH);
     delay(3000);
     digitalWrite(redLED, LOW);
}

void reconnect() {
  // Loop until connected
  //while (!client.connected()) {
    // Create a random client ID
    String Id = "ESP8266Client-";
    Id += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(Id.c_str())) {
      // Once connected, resubscribe
      client.subscribe("System1/wheelchair/move");
      client.subscribe("System1/room/appliances/control");
      Connected = true;
    } 
  //}
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(alarmPin, OUTPUT);
  pinMode(firePin, INPUT);
  dht.begin();
}

void loop() {
  activeAlarm = false;
  //**********************************
  // put your main code here, to run repeatedly:
  if(Serial.available()>=14) //Wait for 1 bytes to arrive
  {
    byte b1 = Serial.read();
    byte b2 = Serial.read();
    bTemp = b1 * 256 + b2;

    b1 = Serial.read();
    b2 = Serial.read();
    HBR = b1 * 256 + b2;
   
    b1 = Serial.read();
    b2 = Serial.read();
    lightSwitch = b1 * 256 + b2;


    b1 = Serial.read();
    b2 = Serial.read();
    fanSwitch = b1 * 256 + b2;

    b1 = Serial.read();
    b2 = Serial.read();
    acSwitch = b1 * 256 + b2;

    b1 = Serial.read();
    b2 = Serial.read();
    door = b1 * 256 + b2;
    
    b1 = Serial.read();
    b2 = Serial.read();
    help = b1 * 256 + b2;

    sendViaWiFi();
  }

  if(WiFi.status() != WL_CONNECTED && (millis() - tryNow > 300000)){
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
  }

  if (!client.connected() && WiFi.status() == WL_CONNECTED) {
    Connected = false;
    reconnect();
  }
  client.loop();
  
  fireDetection();

  gasDetection();

  tempDetection();

  if(millis()-lastMsg > 30000){
    sendSerial();
    sendViaWiFi();
    lastMsg = millis();
  }
  
  delay(1000);
    
}

// detect the exestance of fire
void fireDetection(){
  // retreive fire sensor signal
  if(fireReading != digitalRead(firePin)){
    fireReading = digitalRead(firePin);
    switch(fireReading){
      case 0: 
          alarm();
          Serial.write(89); //Y
          Serial.write(70); //F
          Serial.write(82); //R
          //Serial.println("*****************************");
          delay(100);
          /*Serial.println("** Close Fire **");*/
        break;
            
      case 1: 
          Serial.write(78); //N
          Serial.write(70); //F
          Serial.write(82); //R
          //Serial.println("#############################33");
          delay(100);
          activeAlarm = false;
          /*Serial.println("** no Close Fire **");*/
        break;
    }
    sprintf (msg, "%1d", fireReading);
    client.publish("System1/room/fire", msg);
    delay(500); 
  }
}


void gasDetection(){
  if(gas != digitalRead(gasPin))
  {
    gas = digitalRead(gasPin);
    switch(gas){
      case 0: 
          alarm();
          gas = 0;
          sprintf (msg, "%1d", gas);
          client.publish("System1/room/gas", msg);
          delay(500);
          client.publish("System1/room/appliances/control", "n2");
          delay(200);
          client.publish("System1/room/appliances/control", "n4");
          delay(200);
          Serial.write(89); //Y
          Serial.write(71); //G
          Serial.write(83); //S
          /*Serial.println("** Close gas **");*/
          break;

      case 1:
          sprintf (msg, "%1d", gas);
          client.publish("System1/room/gas", msg);
          delay(500);
          Serial.write(78); //N
          Serial.write(71); //G
          Serial.write(83); //S
          activeAlarm = false;
          /*Serial.println("** no Close gas **");*/
          break;
    }
  }
}

void tempDetection(){
  humidity = dht.readHumidity();
  delay(500);
  temperature = dht.readTemperature();
  
  if((humidity > 85 || humidity < 30 || temperature > 35 || temperature < 10) && humidity != -1){
    alarm();
    dtostrf(temperature, 4, 2, msg);
    client.publish("System1/room/temperature", msg);
    delay(500);
    dtostrf(humidity, 4, 2, msg);
    client.publish("System1/room/humidity", msg);
    delay(500);
    Serial.write(68); //D
    Serial.write(84); //T
    delay(100);

    Serial.write(84);
  Serial.write(temperature / 256);
  Serial.write(temperature % 256);
  delay(100);
  Serial.write(72);
  Serial.write(humidity / 256);
  Serial.write(humidity % 256);
  delay(100);

  if(temperature > 35){
    client.publish("System1/room/appliances/control", "n3");
    delay(200);
  }
   
  }
  else if(!activeAlarm){
    digitalWrite(redLED, LOW);
    digitalWrite(alarmPin, LOW);
    noTone(alarmPin);
  }
}

void alarm(){
  digitalWrite(redLED, HIGH);
  digitalWrite(alarmPin, HIGH);
  tone(alarmPin, 20000);
  activeAlarm = true;
}


/****************** MQResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/
void sendSerial(){
  Serial.write(84);
  Serial.write(temperature / 256);
  Serial.write(temperature % 256);
  delay(100);
  Serial.write(72);
  Serial.write(humidity / 256);
  Serial.write(humidity % 256);
  delay(100);
  if(Connected){
    Serial.write(67); //C
    Serial.write(78); //N
  }else
  {
    Serial.write(85); //U
    Serial.write(67); //C
  }
  delay(100);    
}

void sendViaWiFi(){
  sprintf (msg, "%1d", fireReading);
  client.publish("System1/room/fire", msg);
  delay(500);
  sprintf (msg, "%1d", gas);
  client.publish("System1/room/gas", msg);
  delay(500);
  fireDetection();
  gasDetection();
  sprintf (msg, "%ld", HBR);
  client.publish("System1/body/pulse", msg);
  dtostrf(bTemp, 4, 2, msg);
  client.publish("System1/body/temperature", msg);
  delay(500);
  dtostrf(temperature, 4, 2, msg);
  client.publish("System1/room/temperature", msg);
  delay(500);
  fireDetection();
  gasDetection();
  dtostrf(humidity, 4, 2, msg);
  client.publish("System1/room/humidity", msg);
  delay(500);
  sprintf (msg, "%1d", lightSwitch);
  client.publish("System1/room/light/update", msg);
  delay(500);
  fireDetection();
  gasDetection();
  sprintf (msg, "%1d", fanSwitch);
  client.publish("System1/room/fan/update", msg);
  delay(500);
  sprintf (msg, "%1d", acSwitch);
  client.publish("System1/room/ac/update", msg);
  delay(500);
  fireDetection();
  gasDetection();
  sprintf (msg, "%1d", door);
  client.publish("System1/room/door/update", msg);
  delay(500);
  sprintf (msg, "%ld", help);
  client.publish("System1/ask/help", msg);
}

