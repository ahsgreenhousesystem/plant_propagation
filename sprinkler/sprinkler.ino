const int zoneOne   = 23; 
const int zoneTwo   = 25;
const int zoneThree = 27;
const int zoneFour  = 29;
const int zoneFive  = 31;
const int zoneSix   = 45;

const int timeDelay = 1000;     // delay in ms -- important -- relays wear out if driven too fast

void setup()
{
  //set zone pins as output 
  pinMode(zoneOne,   OUTPUT);  
  pinMode(zoneTwo,   OUTPUT);
  pinMode(zoneThree, OUTPUT);
  pinMode(zoneFour,  OUTPUT);
  pinMode(zoneFive,  OUTPUT);
  pinMode(zoneSix,   OUTPUT);
}


void loop()                    
{
  sprinklersOn(zoneOne);
  sprinklersOff(zoneOne);
} 


void sprinklersOn(int pin) {
  digitalWrite(pin, HIGH);      // turn relay on
  delay(timeDelay);             // wait for one second
}

void sprinklersOff(int pin) {
  digitalWrite(pin, LOW);       // turn relay off
  delay(timeDelay);             // wait for one second
}
