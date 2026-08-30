//RFID
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10  //Peripheral select pin
#define RST_PIN 9  //reset pin
MFRC522 mfrc522(SS_PIN, RST_PIN);  // instantiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;    // create a MIFARE_Key struct named 'key', which will hold the card information
//this is the block number we will write into and the read
int block = 2;
byte blockcontent[16] = {"Last-Minute-Engg"}; //an array with 16 bytes to be written into one of the 64 card blocks is defined.
//byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  // all zeros. This can be used to delete a block.
//This array is used for reading out a block.
byte readbackblock[18];

//Ultrasonic Sensor
const int trig = 5;
const int echo = 4;

//Motor 1 and Enable pin
const int In1 = 9;
const int In2 = 8;
const int En1 = 10;

//Motor 2 and Enable pin
const int In3 = 7;
const int In4 = 6;
const int En2 = 11;

// Colour Sensor
#define S0 A0
#define S1 A1
#define S2 A2
#define S3 A3
#define sensorOut A4
#define OE A5
int frequency = 0;

//Line Sensor Array
#include <QTRSensors.h>
QTRSensors qtr;
const uint8_t SensorCount = 1;
uint16_t sensorValues[SensorCount];

void setup() {

  // RFID
  SPI.begin();
  mfrc522.PCD_Init();    //Init MFRC522 card (PCD means: Proximity coupling device)
  Serial.println("Scan a MIFARE Classic card");
  // Prepare the sercurity key for the read and write functions.
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;    //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  
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
  //digitalWrite(OE, LOW);  //This may or may not be needed...
  
  //Line Sensor Array
  //Configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){A0}, SensorCount);
  
  //Serial Monitor
  Serial.begin(9600);
}

void loop() {

  // RFID
  //look for new cards
  if (! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println("card selected");

  //Select one of the cards
  if (! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.println("Card selected");

  //The blockcontent array is written into the card black
  writeBlock(block, blockcontent);

  //read the block back
  readBlock(block, readbackblock);
  //uncomment below line if you want to see the entire 1k memeory with the block written into it
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  //print the block contents
  Serial.print("read block: ");
  for (int j = 0; j<16; j++) {
    Serial.write(readbackblock[j]);
  }
  Serial.println("");
  
  // Motors
  goStraight();
  
  // Colour Sensor
  //Setting red filtered pohotodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  //Reading the output frequency
  frequency = pulseIn(sensorOut, LOW);
  // Printing the value on the serial monitor
  Serial.print("R= "); // Printing name
  Serial.print(frequency); // printing RED colour frequency
  Serial.print("  ");

  // Line Sensor Array
  //read raw sensor values
  qtr.read(sensorValues);

  //print the snesor values as numbers from 0 to 1023, where 0 means maximum
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
  analogWrite(En1, 200);

  //Turn on Motor 2
  digitalWrite(In3, HIGH);
  digitalWrite(In4, LOW);
  // Set speed between 150 and 255
  analogWrite(En2, 200);
}

//Write specific block
int writeBlock(int blockNumber, byte arrayAddress[]){
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number = blockNumber/4*4;
  int trailerBlock = largestModulo4Number+3; //determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber); Serial.println(" is a trailer block:");return 2;}
  Serial.print(blockNumber);
  Serial.println(" is a data block:");

  //authentication of the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3; // return "3" as error message  
  }

  //wirting the block
  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);
  // status = mfrc522.MIFARE_Write(9, valueBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4; // return "4" as error message
  }
  Serial.println("block was written");
}

//Read specific block
int readBlock(int blockNumber, byte arrayAddress[]){
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock = largestModulo4Number+3; // determine trailer block for the sector

  //authentication for the desired block for access
  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed (read): ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 3; //return "3" as error message
  }

  //reading a block
  byte buffersize = 18; //we need to define a variable with the readbuffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size...
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize); //&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer insteac of just a number
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 4; // return "4" as error message
  }
  Serial.print("block was read");
}
