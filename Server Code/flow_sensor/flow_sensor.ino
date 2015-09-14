/* read liquid flow rate through water flow sensor */

#define FLOWSENSORPINZONE1 2 //pin location for sensor
#define SENSORINTERRUPTZONE1 0 //0 = digital pin 2

volatile int pulseCounter = 0; //counts rising edge of signal
float flowRate = 0; //water flow rate in liters/hour

void setup() {
  Serial.begin(9600);
  Serial.println("Flow sensor test");
  
  pinMode(FLOWSENSORPINZONE1, INPUT); //initializes sensor pin as input
  //digitalWrite(FLOWSENSORPINZONE1, HIGH); //Note: may not need
  attachInterrupt(SENSORINTERRUPTZONE1, pulseCount, RISING); //attach interrupt
}

void loop() {
  pulseCounter = 0; //reset counter
  sei(); //enable interrupts
  delay(1000); //1 second wait
  cli(); //disable interrupts
  
  //(pulseFrequency)/7.5Q = flow rate in L/min
  //7.5 value is approximate, may need calibration
  flowRate = (pulseCounter / 7.5); 
  
  Serial.print(flowRate, DEC); //prints calculated flowRate
  Serial.println(" L/min"); //prints units
}

/* Interrupt Routine */
void pulseCount(){
  pulseCounter++;
}
