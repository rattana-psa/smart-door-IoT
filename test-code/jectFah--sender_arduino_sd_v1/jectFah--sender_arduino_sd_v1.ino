//////////////////////////////////////////////////////////////
//sd card part
#include <SPI.h>
#include <SD.h>
const int chipSelect = 4;
/////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>
SoftwareSerial chat(7, 8); // RX, TX
int i;
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
  chat.begin(4800);
  
////////////////////////////////////////////////////////////////
//sd card part
Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
////////////////////////////////////////////////////////////////
  
}

void loop() {
  // put your main code here, to run repeatedly:


  
int a ;
int b ;
int num1;
int num2;
int state = 0;
int check;
a = analogRead(A0) ;
//Serial.print("a = ");
Serial.println(a);
b = analogRead(A1) ;
//Serial.print("b = ");
Serial.println(b);
if(check != 1){
if(a<200){
  num1 = 2;
  num2 = 0;
  check = 1;
}
else if(b<200){
  num1 = -1;
  num2 = 0;
  check = 1;
}}
delay(150);
a = analogRead(A0) ;
//Serial.print("a = ");
//Serial.println(a);
b = analogRead(A1) ;
//Serial.print("b = ");
//Serial.println(b);
if(check==1){
if(a<200){
  num2 = 2;
}
else if(b<200){
  num2 = -1;
}}
if(num1!=0&&num2!=0){
state = num1 - num2; 
num1 = 0;
num2 = 0;
}
Serial.print("state = ");
Serial.println(state);
Serial.print("num1 = ");
Serial.println(num1);
Serial.print("num2 = ");
Serial.println(num2);
/*if(state == 3){
  Serial.println("somebody come in");
  delay(1000);
  check = 0;
  }
else if(state == -3){
  Serial.println("somebody come out");
  delay(1000);
  check = 0;
  }*/

File dataFile = SD.open("datalog.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    if(state == 3)
      dataFile.println("there is someone come in.");
    else if(state == -3)
      dataFile.println("there is someone come out.");
    dataFile.close();
    // print to the serial port too:
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  
  if(state == 3||state == -3){
  check = 0;
  if (chat.readString()){
     // chat.print(1);
     if(chat.readString()== "Question1"){ //มีการถามคำถาม Question1 ส่งข้อมูลตัวแปร a ออกไป
      chat.print(state);
     }
      Serial.print("Send = ");
      Serial.println(i);
  }

  i++;

  
  delay(100);}
}
