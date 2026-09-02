//Ultrasonic Sensor
const int trig = 5;
const int echo = 4;

//Motor 1 and Enable pin
const int In1 = 9;
const int In2 = 8;
const int En1 = 10;

//Motor 2 and Enable pin
const int In3 = 11;
const int In4 = 12;
const int En2 = 13;

// Colour Sensor
#define S0 A0
#define S1 A1
#define S2 A2
#define S3 A3
#define sensorOut A4
#define OE A5
int frequency = 0;

int redMin = 10,   redMax = 200;
int greenMin = 10, greenMax = 200;
int blueMin = 10,  blueMax = 200;

int redValue, greenValue, blueValue;


//Line Sensor Array
#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 4;
uint16_t sensorValues[SensorCount];

void setup() {

  //Serial Monitor
  Serial.begin(9600);
  
  // ULTRASONIC Sensor
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  
  // Motor 1 and Enable pin
  pinMode(In1, OUTPUT);
  pinMode(In2, OUTPUT);
  pinMode(En1, OUTPUT);
  
  // Motor 2 and Enable pin
  pinMode(In3, OUTPUT);
  pinMode(In4, OUTPUT);
  pinMode(En2, OUTPUT);
  
  // Colour Sensor
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(sensorOut, INPUT);
  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  digitalWrite(OE, LOW);  //This may or may not be needed...
  
  //Line Sensor Array
  //Configure the sensors
  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){2,26,27,28}, SensorCount);
  

}

void loop() {

  // Motors
  goStraight();
  
  // Colour Sensor
  //Setting red filtered pohotodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  //Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Flipping from pulse width duration to frequency
  redValue = map(frequency, redMin, redMax, 255, 0);
  redValue = constrain(redValue, 0, 255);
  // Printing the value on the serial monitor
  Serial.print("R= "); // Printing name
  Serial.print(redValue); // printing Red colour frequency
  Serial.print("  ");

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  //Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Flipping from pulse width duration to frequency
  greenValue = map(frequency, greenMin, greenMax, 255, 0);
  greenValue = constrain(greenValue, 0, 255);
  // Printing the value on the serial monitor
  Serial.print("G= "); // Printing name
  Serial.print(greenValue); // printing Green colour frequency
  Serial.print("  ");

  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  //Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  //Flipping from pulse width duration to frequency
  blueValue = map(frequency, blueMin, blueMax, 255, 0);
  blueValue = constrain(blueValue, 0, 255);
  // Printing the value on the serial monitor
  Serial.print("B= "); // Printing name
  Serial.print(blueValue); // printing Blue colour frequency
  Serial.print("  ");
  
  // Line Sensor Array
  //read raw sensor values
  qtr.read(sensorValues);

  //print the sensor values as numbers from 0 to 1023, where 0 means maximum
  // reflectance and 1023 means minimum reflectance
  for (uint8_t i = 0; i < SensorCount; i++) {
    Serial.print(sensorValues[i]);  
    Serial.print('\t');
  }
  Serial.println();
  
  // Ultrasonic Sensor
  long duration, cm;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  cm = microsecondsToCentimeters(duration);

  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(100);
}

float microsecondsToCentimeters(long microseconds){
  return float(microseconds) / 29 / 2;
}

void goStraight() {
  
  //Turn on Motor 1
  digitalWrite(In1, HIGH);
  digitalWrite(In2, LOW);
  // Set speed between 150 and 255
  analogWrite(En1, 150);

  //Turn on Motor 2
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  // Set speed between 150 and 255
  analogWrite(En2, 150);
}
