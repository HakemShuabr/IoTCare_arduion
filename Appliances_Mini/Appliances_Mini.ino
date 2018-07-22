

const int light = 11;
const int fan = 10;
const int ac = 4;
const int door = 5;
const int alarm = 3;
const int led = 12;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
   pinMode(light, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(ac, OUTPUT);
  pinMode(door, OUTPUT);
  pinMode(alarm, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(light, HIGH);
  pinMode(fan, HIGH);
  pinMode(ac, HIGH);
  pinMode(door, HIGH);
  tone(alarm, 2000, 500);
  digitalWrite(led, HIGH);
  delay(1000);
  digitalWrite(led, LOW);
  delay(2000);

}

void loop() {
  // put your main code here, to run repeatedly:

   
}
void serialEvent(){
  if(Serial.available()){
   digitalWrite(led, HIGH);
   tone(alarm, 2000, 500);
   switch((char)Serial.read()){
      case 'l':
        digitalWrite(light, HIGH);
        break;
        
      case 'f':
        digitalWrite(fan, HIGH);
        break;
      
      case 'c':
        digitalWrite(ac, HIGH);
        break;
                  
      case 'd':
        digitalWrite(door, HIGH);
        break;

      case 'a':
        digitalWrite(light, HIGH);
        digitalWrite(fan, HIGH);
        digitalWrite(ac, HIGH);
        digitalWrite(door, HIGH);
        break;
      case 'L':
        digitalWrite(light, LOW);
        break;
        
      case 'F':
        digitalWrite(fan, LOW);
        break;
      
      case 'C':
        digitalWrite(ac, LOW);
        break;
                  
      case 'D':
        digitalWrite(door, LOW);
        break;

      case 'A':
        digitalWrite(light, LOW);
        digitalWrite(fan, LOW);
        digitalWrite(ac, LOW);
        digitalWrite(door, LOW);
        break;
    }
    delay(1000);
    digitalWrite(led, LOW);
  }
}

