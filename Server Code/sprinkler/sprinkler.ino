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
  sprinklerOff(zone1);

  pinMode(zone2, OUTPUT);
  sprinklerOff(zone2);

  pinMode(zone3, OUTPUT);
  sprinklerOff(zone3);

  pinMode(zone4, OUTPUT);
  sprinklerOff(zone4);

  pinMode(zone5, OUTPUT);
  sprinklerOff(zone5);

  pinMode(zone6, OUTPUT);
  sprinklerOff(zone6);

  pinMode(zone7, OUTPUT);
  sprinklerOff(zone7);
}

void loop()
{
  sprinklerOff(zone4);
  sprinklerOn(zone4);
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
