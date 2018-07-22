#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

RF24 radio(4, 15); // CE, CSN
const byte address[6] = "00001";

/////////////variables/////////////
long tryNow = 0;
bool Connected = false;
const int wifiLed = 5;


////////////WiFi//////////////////

const char* ssid = "Lnet";//"AndroidAP";//"sbahasan@unifi";//  //
const char* password = "87654321";//  //"0183887285";
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastUpdate = 0;
char msg[50];


void setup_wifi(){

  delay(10);
  
  WiFi.begin(ssid, password);
  tryNow = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - tryNow < 15000)) {
    yield();
    delay(500);
    //Serial.print(".");
  }

  if(WiFi.status() == WL_CONNECTED){
    Connected = true;
    digitalWrite(wifiLed, HIGH);
  }
  else{
    Connected = false;
    digitalWrite(wifiLed, LOW);
  }
  
  randomSeed(micros());

}

void callback(char* topic, byte* payload, unsigned int length) {
    char text[30]="";
    for(int i = 0; i < length; i++){
       text[i] = (char)payload[i];
    }
    sendSerial(text);
}

void reconnect() {
// Loop until connected
  String Id = "ESP8266Client-";
  Id += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(Id.c_str())) {
    // Once connected, resubscribe
    client.subscribe("System1/room/appliances/control");
    Connected = true;
    digitalWrite(wifiLed, HIGH);
  } 
}

void setup() {
  Serial.begin(9600);
  pinMode(wifiLed, OUTPUT);
  digitalWrite(wifiLed, LOW);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15, 15);
  radio.startListening();
}
void loop() {
  if(WiFi.status() != WL_CONNECTED && (millis() - tryNow > 300000)){
    digitalWrite(wifiLed, LOW);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    
  }

  if (!client.connected() && WiFi.status() == WL_CONNECTED) {
    Connected = false;
    digitalWrite(wifiLed, LOW);
    reconnect();
  }
  client.loop();
      
  if (radio.available()) {
    char text[30] = "";
    radio.read(&text, sizeof(text));
    char *data = text;
    if(text[0] == 'n' || text[0] == 'f')
      sendSerial(text);
  }

  delay(1000);
}

void sendSerial(char text[]){
   char *data = text;
   if(text[0] == 'n'){
      switch(text[1]){
        case '1':
          Serial.write(76);
          delay(500);
          sprintf (msg, "%1d", 1);
          client.publish("System1/room/light/update", msg);
          break;
          
        case '2':
          Serial.write(70);
          delay(500);
          sprintf (msg, "%1d", 1);
          client.publish("System1/room/fan/update", msg);
          break;
        
        case '3':
          Serial.write(67);
          delay(500);
          sprintf (msg, "%1d", 1);
          client.publish("System1/room/ac/update", msg);
          break;
                    
        case '4':
          Serial.write(68);
          delay(500);
          sprintf (msg, "%1d", 1);
          client.publish("System1/room/door/update", msg);          
          break;

        case '5':
          Serial.write(65);
          delay(500);
          sprintf (msg, "%1d", 1);
          client.publish("System1/room/light/update", msg);
          delay(500);
          client.publish("System1/room/fan/update", msg);
          delay(500);
          client.publish("System1/room/ac/update", msg);
          delay(500);
          client.publish("System1/room/door/update", msg);          
          break;
     }
  }
    
    else if(text[0] == 'f'){
      switch(text[1]){
        case '1':
          Serial.write(108);
          delay(500);
          sprintf (msg, "%1d", 0);
          client.publish("System1/room/light/update", msg);
          break;
          
        case '2':
          Serial.write(102);
          delay(500);
          sprintf (msg, "%1d", 0);
          client.publish("System1/room/fan/update", msg);
          break;
        
        case '3':
          Serial.write(99);
          delay(500);
          sprintf (msg, "%1d", 0);
          client.publish("System1/room/ac/update", msg);
          break;
                    
        case '4':
          Serial.write(100);
          delay(500);
          sprintf (msg, "%1d", 0);
          client.publish("System1/room/door/update", msg);
          break;

        case '5':
          Serial.write(97);
          delay(500);
          sprintf (msg, "%1d", 0);
          client.publish("System1/room/light/update", msg);
          delay(500);
          client.publish("System1/room/fan/update", msg);
          delay(500);
          client.publish("System1/room/ac/update", msg);
          delay(500);
          client.publish("System1/room/door/update", msg);          
          break;
      }
   }
}

