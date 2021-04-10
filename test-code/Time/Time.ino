#include <ESP8266WiFi.h>
#include <time.h>
#include <SPI.h>
#include <SD.h>

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 4;

const char* ssid = "vivo V3";                  //ใส่ชื่อ SSID Wifi
const char* password = "25022503";          //ใส่รหัสผ่าน

int timezone = 7 * 3600;                    //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0;                                //กำหนดค่า Date Swing Time

void setup()
{
 Serial.begin(115200);
 Serial.setDebugOutput(true);

  WiFi.mode(WIFI_STA);                                        //เชื่อมต่อ Wifi
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
     Serial.print(",");
     delay(1000);
   }
    configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");     //ดึงเวลาจาก Server
    Serial.println("\nWaiting for time");
    while (!time(nullptr)) {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("");

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
}
        
void loop()
{
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");    //ดีงเวลาปัจจุบันจาก Server อีกครั้ง
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  int d = p_tm->tm_mday , m = p_tm->tm_mon + 1;
  int h = p_tm->tm_hour , mi = p_tm->tm_min;
  
  Serial.print("Date: "); 
  Serial.print(d);  Serial.print("/");  Serial.println(m);
  
  Serial.print("Time: ");
  Serial.print(h);  Serial.print(".");  Serial.println(mi);
  
  delay(1000);
}

void recordTime(int d, int m, int h, int mi){
  myFile = SD.open("time.txt", FILE_WRITE); // เปิดไฟล์ที่ชื่อ test.txt เพื่อเขียนข้อมูล โหมด FILE_WRITE
  
  // ถ้าเปิดไฟล์สำเร็จ ให้เขียนข้อมูลเพิ่มลงไป
  if (myFile) {
    Serial.print("Writing to test.txt...");
    
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
    Serial.println("error opening test.txt");
    
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

