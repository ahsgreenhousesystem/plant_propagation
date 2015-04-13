const int zone1 = 23; 
const int zone2 = 25;
const int zone3 = 27;
const int zone4 = 29;
const int zone5 = 31;
const int zone6 = 45;
const int zone7 = 47;

const int timeDelay = 5000;                      // delay in ms -- important -- relays wear out if driven too fast

void setup()
{
  //initialize all zone pins
  pinMode(zone1, OUTPUT);
  pinMode(zone2, OUTPUT);
  pinMode(zone3, OUTPUT);
  pinMode(zone4, OUTPUT);
  pinMode(zone5, OUTPUT);
  pinMode(zone6, OUTPUT);
  pinMode(zone7, OUTPUT);
}

void loop()                    
{
  sprinklerOff(zone2);
  sprinklerOn(zone2);
} 

void sprinklerOff(int pin) {
  Serial.println(pin + " was turned OFF.");      // will be put in log
  digitalWrite(pin, HIGH);                       // turn relay off
  delay(timeDelay);                              // wait for five seconds
}

void sprinklerOn(int pin) {
  Serial.println(pin + " was turned ON.");       // will be put in log
  digitalWrite(pin, LOW);                        // turn relay on
  delay(timeDelay);                              // wait for five seconds
}
