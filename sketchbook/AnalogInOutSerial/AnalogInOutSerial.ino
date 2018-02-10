#define SAMPLE_TIME_US 1000
#define BAUD 500000

#define CONV_TIME_US  120

const int analogInPin1 = A1;
const int analogInPin2 = A2;
const int analogOutPin = 3;

int sensorValue1 = 0;
int sensorValue2 = 0; 
int outputValue = 0;

int i = 0;

void setup() {
  //Serial.begin(38400); 
  Serial.begin(500000); 
}

void loop() {
  delayMicroseconds(SAMPLE_TIME_US-CONV_TIME_US*2-480);
  sensorValue1 = analogRead(analogInPin1);
  sensorValue2 = analogRead(analogInPin2);
  // map it to the range of the analog out:
  outputValue = map(sensorValue1, 0, 400, 0, 255);  

  //analogWrite(analogOutPin, outputValue);         

  // print the results to the serial monitor:
  Serial.print(sensorValue1);
  Serial.print("\t");
  Serial.println(sensorValue2);   
  //  delay(1);
}
