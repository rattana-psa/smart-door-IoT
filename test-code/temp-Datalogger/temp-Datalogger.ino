/*
  SD card datalogger

  This example shows how to log data from three analog sensors
  to an SD card using the SD library.

  The circuit:
   analog sensors on analog ins 0, 1, and 2
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4

  created  24 Nov 2010
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

#define MUX_A D4
#define MUX_B D3
#define MUX_C D2

int a1 = 0; 
int ICE = 0;

#define ANALOG_INPUT A0

const int chipSelect = 4;

#define R0 10000
#define B 3435
#define R1 10000
float T0 = 25;

void setup() {
  T0 = T0 + 273.15;
  
  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);     
  pinMode(MUX_C, OUTPUT);    
   
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
}

void changeMux(int c, int b, int a) {
  digitalWrite(MUX_A, a);
  digitalWrite(MUX_B, b);
  digitalWrite(MUX_C, c);
}

void loop() {
  delay(1000);
  int value;
  String dataString = "";
  if (Serial.available() > 0) { 
          a1 = Serial.read();    
          Serial.print("> ");
          Serial.println(a1, DEC);
          if(a1==97) { 
            ICE = 0;            
          }
          else if(a1==98){ 
            ICE = 1;    
          }   
  } 
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
   
  if(ICE==0){ //Ldr sensor
    Serial.println("Sensor 1 ON : LDR ");
    changeMux(LOW, LOW, LOW);
    value = analogRead(ANALOG_INPUT);
    dataString += String(value);
    if (dataFile) {
      dataFile.println("Sensor 1 ON : LDR ");
      dataFile.println(dataString);  
      Serial.println(dataString); 
      Serial.print("Write to datalog.txt......");
      dataFile.close();  
      Serial.println("done!"); 
    }
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  
   else if(ICE==1){ //Temperature sensor
    Serial.println("Sensor 2 ON : Temperature ");
    changeMux(LOW, LOW, HIGH);
    value = analogRead(ANALOG_INPUT);
    dataString += String(value); 
    int sam = value;
    float s = 1023-sam;
    float R = R1*(sam/s);
    float f = (1.0/T0)+(1.0/B)*log(R/R0);
    float T = (1.0/f)-273;
  
    if (dataFile) {
      Serial.print("Analog reading : ");
      Serial.print(sam);
      Serial.print("thermistor R : ");
      Serial.print(R);
      Serial.print("Temp :");
      Serial.print(T);
      Serial.println("ํC");
      dataFile.println("Sensor 2 ON : Temperature ");
      dataFile.print("Analog reading : ");
      dataFile.print(sam);
      dataFile.print("thermistor R : ");
      dataFile.print(R);
      dataFile.print("Temp :");
      dataFile.print(T);
      dataFile.println("ํC");
      Serial.print("Write to datalog.txt......");
      dataFile.close();  
      Serial.println("done!"); 
    }
    else {
      Serial.println("error opening datalog.txt");
    }
  }
  
}
