#define SAMPLE_TIME_US 1000
#define BAUD 500000

#define CONV_TIME_US  120

const int analogInPin1 = A1;
const int analogInPin2 = A2;
const int analogOutPin = 3;

int sensorValue1 = 0;
int sensorValue2 = 0; 
int outputValue = 0;

unsigned long int i = 0;

unsigned long start_us = 0;
unsigned long end_us = 0;
unsigned long used_us = 0;

void setup() {
  //Serial.begin(38400); 
  Serial.begin(500000);
  // Create a reference signal
  // According to Arduino documentation, the frequency is about 490 Hz
  analogWrite(analogOutPin, 127);
}

void loop() {
  start_us = micros();
  Serial.print(used_us);
  Serial.print("\t");
  Serial.print(analogRead(analogInPin1));
  Serial.print("\t");
  Serial.print(analogRead(analogInPin2));
  Serial.println();
  
  // Measure time
  end_us = micros();
  used_us = end_us - start_us;
  
  // Idle
  while( (micros()-start_us) < SAMPLE_TIME_US );
}
