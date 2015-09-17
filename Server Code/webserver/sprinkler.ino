void setupSprinklers() {
  // Initialize all zone pins
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

void sprinklerOff(int pin) {
  digitalWrite(pin, HIGH);
  delay(timeDelay);
}

void sprinklerOn(int pin) {
  digitalWrite(pin, LOW);
  delay(timeDelay);
}

void sprinkler1On() {
  sprinklerOn(zone1);
}

void sprinkler1Off() {
  sprinklerOff(zone1);
}

void sprinkler2On() {
  sprinklerOn(zone2);
}

void sprinkler2Off() {
  sprinklerOff(zone2);
}

void sprinkler3On() {
  sprinklerOn(zone3);
}

void sprinkler3Off() {
  sprinklerOff(zone3);
}

void sprinkler4On() {
  sprinklerOn(zone4);
}

void sprinkler4Off() {
  sprinklerOff(zone4);
}

void sprinkler5On() {
  sprinklerOn(zone5);
}

void sprinkler5Off() {
  sprinklerOff(zone5);
}

void sprinkler6On() {
  sprinklerOn(zone6);
}

void sprinkler6Off() {
  sprinklerOff(zone6);
}

void sprinkler7On() {
  sprinklerOn(zone7);
}

void sprinkler7Off() {
  sprinklerOff(zone7);
}
