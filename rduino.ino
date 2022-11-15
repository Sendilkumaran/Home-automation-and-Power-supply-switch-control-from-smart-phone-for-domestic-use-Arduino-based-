
#include <NewPing.h>
#define MAX_DISTANCE 150

class USSensor {
public:
  USSensor(int trigPin, int echoPin, String aName) {
    trig = trigPin;
    echo = echoPin;
    sonar = new NewPing(trigPin, echoPin, MAX_DISTANCE);
    isActivated = false;
    name = aName;
    time = 0;
  }
  
  void update() {
    updateSensor();
    if (value < 100) {
      setActive(true);
    } else {
      if (millis() - time > 500) {
        setActive(false);
      } 
    }
  }
  
  void reset() {
    setActive(false);
    time = 0;
  }
  
  void setActive(boolean state) {
    if (!isActivated && state) {
      time = millis();
    }
    if (isActivated != state) {
      isActivated = state;
    }
  }
  
  boolean wasActive() {
    return (time != 0) && !isActivated;
  }
  
  int lastActive() {
    return millis() - time;
  }
  
private:
  int trig;
  int echo;
  int led;
  String name;
  boolean isActivated;
  unsigned long time;
  float value;
  NewPing *sonar;
  
  void updateSensor() {
    
    delay(25);
    unsigned int uS = sonar->ping();
    value = uS / US_ROUNDTRIP_CM;
    
    delay(5);
    pinMode(trig, OUTPUT);
    digitalWrite(trig, LOW);
    delayMicroseconds(50);
    digitalWrite(trig, HIGH);
    delayMicroseconds(200);
    digitalWrite(trig, LOW);
    pinMode(echo, INPUT);
    int pulseLen = pulseIn(echo, HIGH);
    float currentValue = pulseLen / 29.387 /2; // [voloshyn] convert into cm
    if (value == 0.0f) value = currentValue;
    else value = (value + currentValue) / 2;
   value = pulseLen / 29.387 /2; // [voloshyn] convert into cm
  }
};

int peopleInRoom = 0;
USSensor *A;
USSensor *B;


char switchCode;
int relayLight1 = 2;
int relayfan = 3;
int Light1 = LOW;
int Light2 = LOW;
int pirPin = 4;
int noMotionIter = 0;
int us1Pin = 7;
int us2Pin = 6;
int startCon = 1;
int reading;
float voltage;
float temp;
int LM=A0;
int fan=LOW;


void setup() {
  // put your setup code here, to run once:
  pinMode(relayLight1, OUTPUT);
  pinMode(relayfan, OUTPUT);
  Serial.begin(9600);      //terminal prompt
  delay(50);
  A = new USSensor(us1Pin, us1Pin, "A");
  B = new USSensor(us2Pin, us2Pin, "B");
  Serial.println("Now you can control switches from phone.");
  pinMode(relayfan,OUTPUT);
  Serial.println("LM35 is ready");
  delay(50);
}

void loop() {

  

  
  // put your main code here, to run repeatedly:
   while (true){



    //below code (until delay command) : for check movement if there is no movement it turned light1 off
    if (Light1 == HIGH && digitalRead(pirPin) == LOW ){
      noMotionIter++;   //count every loop time with no motion
      if (noMotionIter == 100){ //this check no motion for 10s 
        Serial.println("Light 1 automatically turned off because of no Movement.");
        digitalWrite(relayLight1, LOW);
        Light1=LOW;
        noMotionIter = 0;
        }
      }
    if (digitalRead(pirPin)== HIGH){    //if there is a motion detected stop count No motion duration.
      noMotionIter = 0;
      }
      delay(50);



    A->update();
    B->update();
    
    if (A->wasActive() && B->wasActive()) {
      int a_time = A->lastActive();
      int b_time = B->lastActive();
      
      if (a_time < 5000 && b_time < 5000) {
        if (a_time > b_time) {
          peopleInRoom++;
        } else {
          peopleInRoom--;
        }
        if (peopleInRoom < 0) { // [voloshyn] in case when someone was already in the room when system was activated
          peopleInRoom = 0;
          }
        Serial.print("People in room: ");
        Serial.println(peopleInRoom);
      }
      A->reset();
      B->reset();
    }

    


    if (peopleInRoom > 0){
      startCon=0;
      }
    
    if (peopleInRoom == 0 & startCon == 0 ) {
      digitalWrite(relayLight1, LOW);
      Serial.println("Light 1 turned of because of count is Zero");
      startCon=1;
     }
      
    reading=analogRead(LM);
    voltage=reading*(5000/1024);
    temp=voltage/10;
    
    if (temp>25){
      fan=HIGH;
      //Serial.print("Temperature=  ");
      //Serial.println(temp);
    }
  
    else{
      fan=LOW;
      //Serial.print("Temperature=  ");
      //Serial.println(temp);
    }
  
    digitalWrite(relayfan,fan);


         
    if (Serial.available()){  // if there is any code entry thi will break and run into code
      break;
    }
  }

    

  switchCode=Serial.read();

  if (switchCode == '1'){
    Light1 = HIGH;
    Serial.println("Light 1 turned on via blutooth");
    }

  if (switchCode == '2'){
    Light2 = HIGH;
    Serial.println("Fan turned on via blutooth");
    }

  if (switchCode == '0'){
    Light1 = LOW;
    Serial.println("Light 1 turned off via blutooth");
    }

  if (switchCode == '9'){
    Light2 = LOW;
    Serial.println("Fan turned off via blutooth");
    }
  
  if (switchCode == '3'){
    Light1 = HIGH;
    Light2 = HIGH;
    Serial.println("All Lights turned on via blutooth");
    }

  if (switchCode == '8'){
    Light1 = LOW;
    Light2 = LOW;
    Serial.println("All Lights turned off via blutooth");
    }



  
  digitalWrite(relayLight1, Light1);
  digitalWrite(relayfan, Light2);
  delay(50);
}
