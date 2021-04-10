#include <SPI.h>
#include <SD.h>

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 4;

void setup()
{

  Serial.begin(9600);
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
  
  myFile = SD.open("test.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE
  
  // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3."); // สั่งให้เขียนข้อมูล
    myFile.close(); // ปิดไฟล์
    Serial.println("done.");
  } else {
    // ถ้าเปิดไฟลืไม่สำเร็จ ให้แสดง error 
    Serial.println("error opening test.txt");
  }
  
  // เปิดไฟล์เพื่ออ่าน
  myFile = SD.open("test.txt"); // สั่งให้เปิดไฟล์ชื่อ test.txt เพื่ออ่านข้อมูล
  if (myFile) {
    Serial.println("test.txt:");
    // อ่านข้อมูลทั้งหมดออกมา
    while (myFile.available()) {
    Serial.write(myFile.read());
    }
    myFile.close(); // เมื่ออ่านเสร็จ ปิดไฟล์
  } else {
  // ถ้าอ่านไม่สำเร็จ ให้แสดง error 
    Serial.println("error opening test.txt");
  }
}

void loop()
{

}
