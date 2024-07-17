// VARS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~`
// textile
// resistive sensors
const int sensorPin = A1; 

// const int sensorPin2 = A2; 

// capacitive sensors
const int capSensePin = 4;
const int capSensePin2 = 3;

// const int capSensePin3 = 4;
// const int capSensePin4 = 5;

const int bufferSize = 10; // creating a buffer
int sensorValues[bufferSize]; // storing buffer size in array

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < bufferSize; i++) {
    sensorValues[i] = 0; // init sensor values array
  }

}

void loop() {
  // read vals & add to buffer
  int sensorVal = analogRead(sensorPin) / 2;
  addToBuffer(sensorVal);

  // storing duration & distance
  long duration;
  
  // calc average of buffer values
  int averageVal = calculateAverage();
  
  // print raw values
  //Serial.print("Raw Value: ");
  //Serial.print(sensorVal);
  // print averaged values
  //Serial.print(", Averaged Value: ");
  Serial.print(readCapacitivePin(capSensePin));

  Serial.print(',');

  Serial.print(readCapacitivePin(capSensePin2));

  Serial.print(',');

  Serial.println(averageVal);
  
  delay(100);
}

void addToBuffer(int newValue) {
  // shift values in the array to the left
  for (int i = 0; i < bufferSize - 1; i++) {
    sensorValues[i] = sensorValues[i + 1];
  }
  
  // add new value to the end of the array
  sensorValues[bufferSize - 1] = newValue;
}

// function to calc average
int calculateAverage() {
  int sum = 0;
  
  // add them up
  for (int i = 0; i < bufferSize; i++) {
    sum += sensorValues[i];
  }
  
  // divide by bufferSize to get average
  int average = sum / bufferSize;
  
  return average;
}

// function to read capacitive sensor vals
// readCapacitivePin
//  Input: Arduino pin number
//  Output: A number, from 0 to 17 expressing
//          how much capacitance is on the pin
//  When you touch the pin, or whatever you have
//  attached to it, the number will get higher
//  In order for this to work now,
// The pin should have a resistor pulling
//  it up to +5v.
uint8_t readCapacitivePin(int pinToMeasure){
  // This is how you declare a variable which
  //  will hold the PORT, PIN, and DDR registers
  //  on an AVR
  volatile uint8_t* port;
  volatile uint8_t* ddr;
  volatile uint8_t* pin;
  // Here we translate the input pin number from
  //  Arduino pin number to the AVR PORT, PIN, DDR,
  //  and which bit of those registers we care about.
  byte bitmask;
  if ((pinToMeasure >= 0) && (pinToMeasure <= 7)){
    port = &PORTD;
    ddr = &DDRD;
    bitmask = 1 << pinToMeasure;
    pin = &PIND;
  }
  if ((pinToMeasure > 7) && (pinToMeasure <= 13)){
    port = &PORTB;
    ddr = &DDRB;
    bitmask = 1 << (pinToMeasure - 8);
    pin = &PINB;
  }
  if ((pinToMeasure > 13) && (pinToMeasure <= 19)){
    port = &PORTC;
    ddr = &DDRC;
    bitmask = 1 << (pinToMeasure - 13);
    pin = &PINC;
  }
  // Discharge the pin first by setting it low and output
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  delay(1);
  // Make the pin an input WITHOUT the internal pull-up on
  *ddr &= ~(bitmask);
  // Now see how long the pin to get pulled up
  int cycles = 16000;
  for(int i = 0; i < cycles; i++){
    if (*pin & bitmask){
      cycles = i;
      break;
    }
  }
  // Discharge the pin again by setting it low and output
  //  It's important to leave the pins low if you want to 
  //  be able to touch more than 1 sensor at a time - if
  //  the sensor is left pulled high, when you touch
  //  two sensors, your body will transfer the charge between
  //  sensors.
  *port &= ~(bitmask);
  *ddr  |= bitmask;
  
  return cycles;
}
