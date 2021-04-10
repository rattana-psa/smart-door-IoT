#include <SoftwareSerial.h>
#include <SPI.h>
#include <SD.h>

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 4;
  
SoftwareSerial ArduinoSerial(3, 2); // RX, TX

void setup() {
  Serial.begin(115200);
  ArduinoSerial.begin(4800);
  while (!Serial) {
    ; // รอจนกระทั่งเชื่อมต่อกับ Serial port แล้ว สำหรับ Arduino Leonardo เท่านั้น
  }  
  
  Serial.print("Initializing SD card...");
  pinMode(SS, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
}

void loop() {
  int d,m,h,mi;
  int count = 1;
  while (ArduinoSerial.available() > 0) {    
    float val = ArduinoSerial.parseFloat();
      if (ArduinoSerial.read() == '\n') {
        switch(count){
          case 1:
            d = ArduinoSerial.parseInt();
            Serial.println("Write D");
            delay(1000);
            break;
          case 2:
            m = ArduinoSerial.parseInt();
            Serial.println("Write m");
            delay(1000);
            break;
          case 3:
            h = ArduinoSerial.parseInt();
            Serial.println("Write h");
            delay(1000);
            break;
          case 4:
            mi = ArduinoSerial.parseInt();
            Serial.println("Write mi");
            count = 0;
            delay(1000);
            break;         
        }
        count++;        
        
    }
    

    myFile = SD.open("time.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE
  
  // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
  if (myFile) {
    Serial.print("Writing to time.txt...");
    
    myFile.print("Date: ");
    myFile.print(d);
    myFile.print("/");
    myFile.println(m); // สั่งให้เขียนข้อมูล

    myFile.print("Time: ");
    myFile.print(h);
    myFile.print(".");
    myFile.println(mi);
    
    myFile.close(); // ปิดไฟล์
    Serial.println("done.");
    
  } else {
    // ถ้าเปิดไฟลืไม่สำเร็จ ให้แสดง error 
    Serial.println("error opening time.txt");
    
  }
  
  // เปิดไฟล์เพื่ออ่าน
  myFile = SD.open("time.txt"); // สั่งให้เปิดไฟล์ชื่อ test.txt เพื่ออ่านข้อมูล
  if (myFile) {
    Serial.println("time.txt:");
    // อ่านข้อมูลทั้งหมดออกมา
    while (myFile.available()) {
      Serial.write(myFile.read());
      
  }
  myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
    
  } else {
    // ถ้าอ่านไม่สำเร็จ ให้แสดง error 
    Serial.println("error opening time.txt");
    
  }
  }
}
