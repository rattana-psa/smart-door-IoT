#include <MicroGear.h>
#include <ESP8266WiFi.h>
#include <time.h>
#include <SPI.h>
#include <SD.h>

int timezone = 7 * 3600;                    //ตั้งค่า TimeZone ตามเวลาประเทศไทย
int dst = 0;                                //กำหนดค่า Date Swing Time

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 4;

void Line_Notify(String message) ;
// Line config
#define LINE_TOKEN "RuqxCNjnyYPDV8IQkRZL3uuqjJr4I4Xay0iSjAc7jJY"
#define SW D1

//ประตูปลดล็อค!
String message = "%E0%B8%9B%E0%B8%A3%E0%B8%B0%E0%B8%95%E0%B8%B9%E0%B8%9B%E0%B8%A5%E0%B8%94%E0%B8%A5%E0%B9%87%E0%B8%AD%E0%B8%84!";
//รหัสผ่านของคุณ คือ
String messageForget = "%E0%B8%A3%E0%B8%AB%E0%B8%B1%E0%B8%AA%E0%B8%9C%E0%B9%88%E0%B8%B2%E0%B8%99%E0%B8%82%E0%B8%AD%E0%B8%87%E0%B8%84%E0%B8%B8%E0%B8%93%20%E0%B8%84%E0%B8%B7%E0%B8%AD";

#define lock D0

const char* ssid = "vivo V3";
const char* password = "25022503";

#define APPID "labServo"
#define KEY "cE7n1be8BLTJLwl"
#define SECRET "08zh2mGyku6tlNOavmezOQGuT"

#define ALIAS "NodeMCU1"
#define TargetWeb "DigitalOUTPUT_HTML_web"
WiFiClient client;
MicroGear microgear(client);

int limit = 5;
char pw[6] = { '0', '0', '0', '0', '0' };
char passIn[6] = { '*', '*', '*', '*', '*' };
char passRe[6] = { '*', '*', '*', '*', '*' };
int check = 0;
int resetPass = 0;
int setNewPassword = 0;
int door = 0;


void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  
  /* Event listener */
  microgear.on(MESSAGE,onMsghandler);
  microgear.on(CONNECTED,onConnected);

  Serial.println("Starting...");

  //Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Time
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");     //ดึงเวลาจาก Server
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");

  //Net
  microgear.init(KEY,SECRET,ALIAS);
  microgear.connect(APPID);

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
    
  pinMode(lock, OUTPUT);
  pinMode(ledPin,OUTPUT);
  
  digitalWrite(ledPin,HIGH); // Turn off LED
  digitalWrite(lock, HIGH);
}

int d, h, m, mi;

void loop()
{
  
  Serial.print("PW: ");
  Serial.println(pw);
  Serial.print("PR: ");
  Serial.println(passRe);
  Serial.print("PI: ");
  Serial.println(passIn);
  Serial.print("DOOR: ");
  Serial.println(door);
  if(microgear.connected())
  {
    microgear.loop();
    Serial.print("-");
  }
  else
  {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
  }

  Serial.println("S.Time");
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");    //ดีงเวลาปัจจุบันจาก Server อีกครั้ง
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  d = p_tm->tm_mday ; m = p_tm->tm_mon + 1;
  h = p_tm->tm_hour ; mi = p_tm->tm_min;
  
  Serial.print("Date: "); 
  Serial.print(d);  Serial.print("/");  Serial.println(m);
  
  Serial.print("Time: ");
  Serial.print(h);  Serial.print(".");  Serial.println(mi);

  Serial.println("*********************** End Time");
  
  
  //รหัสถูก เปิดประตู
  if(door){
    openDoor();
    //recordTime();
    Serial.println("Open Door!!!");      
    check = 0;
    
  }else if(check == 1 && resetPass == 1){
    
    //ส่งไปบอกเว็บว่าให้เปลี่ยนข้อความเป็นให้ใส่รหัสใหม่ 5 ตัว
    microgear.chat(TargetWeb, "newPass");

    setNewPassword = 1;
    resetPass = 0;
    check = 0;
    
  }
  
  delay(300);
  
  
}

int countIn = 0;  //นับว่าใส่รหัสมาแล้วกี่ตัว
int countRe = 0;  //นับรหัสตอนรี
int countNew = 0;  //นับรหัสตั้งใหม่

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen)
{
  Serial.print("Incoming message ‐‐> ");
  Serial.print(topic);
  Serial.print(" : ");
  char strState[msglen];
  for (int i = 0; i < msglen; i++)
  {
    strState[i] = (char)msg[i];
    Serial.print((char)msg[i]);
  }
  Serial.println();
  String stateStr = String(strState).substring(0, msglen);

  if(resetPass){
    Serial.println("PassRe");
    if(stateStr == "1"){
      passRe[countRe] = '1';
    }
    else if (stateStr == "2"){
      passRe[countRe] = '2';
    }
    else if (stateStr == "3"){
      passRe[countRe] = '3';
    }
    else if (stateStr == "4"){
      passRe[countRe] = '4';
    }
    else if (stateStr == "5"){
      passRe[countRe] = '5';
    }
    else if (stateStr == "6"){
      passRe[countRe] = '6';
    }
    else if (stateStr == "7"){
      passRe[countRe] = '7';
    }
    else if (stateStr == "8"){
      passRe[countRe] = '8';
    }
    else if (stateStr == "9"){
      passRe[countRe] = '9';
    }
    else if (stateStr == "0"){
      passRe[countRe] = '0';
    }
    else if (stateStr == "*"){
      Serial.println("Clear");
      countRe = -1;
    }
    else if (stateStr == "#"){
      Serial.println("Enter");
      checkPassword();
      if(check == 0){
        microgear.chat(TargetWeb, "wrongRe");
      }
      
      for(int i = 0; i<5; i++){
        passRe[i] = '*';
      } 
      
    }
    else if (stateStr == "R"){
      Serial.println("Reset Password");
      countRe = -1;
  
      //reset password
      resetPassword();
    }
    else if (stateStr == "F"){
      Serial.println("Forget Password");
      countRe = -1;
      
      //Send to line noti    
      Line_Notify(pw);    
      Serial.println("Send to Line Noti");
    }
    Serial.print("Count Re: ");   Serial.println(countRe);
    Serial.print("Pass Re: ");   Serial.println(passRe);
    countRe++;
    
  }else if(setNewPassword){
    Serial.println("PassNew");
    if(stateStr == "1"){
      pw[countNew] = '1';
    }
    else if (stateStr == "2"){
      pw[countNew] = '2';
    }
    else if (stateStr == "3"){
      pw[countNew] = '3';
    }
    else if (stateStr == "4"){
      pw[countNew] = '4';
    }
    else if (stateStr == "5"){
      pw[countNew] = '5';
    }
    else if (stateStr == "6"){
      pw[countNew] = '6';
    }
    else if (stateStr == "7"){
      pw[countNew] = '7';
    }
    else if (stateStr == "8"){
      pw[countNew] = '8';
    }
    else if (stateStr == "9"){
      pw[countNew] = '9';
    }
    else if (stateStr == "0"){
      pw[countNew] = '0';
    }
    else if (stateStr == "*"){
      Serial.println("Clear");
      countNew = -1;
    }
    else if (stateStr == "#"){
      Serial.println("Enter");
      countNew = -1;
      setNewPassword = 0;
      Serial.println("Set New Password Finished!");
      microgear.chat(TargetWeb, "fin");
         
    }
    Serial.print("Count New: ");   Serial.println(countNew);
    Serial.print("Pass New: ");   Serial.println(pw);
    countNew++;
    
  }else{
    Serial.println("Pass");
    if(stateStr == "1"){
      passIn[countIn] = '1';
    }
    else if (stateStr == "2"){
      passIn[countIn] = '2';
    }
    else if (stateStr == "3"){
      passIn[countIn] = '3';
    }
    else if (stateStr == "4"){
      passIn[countIn] = '4';
    }
    else if (stateStr == "5"){
      passIn[countIn] = '5';
    }
    else if (stateStr == "6"){
      passIn[countIn] = '6';
    }
    else if (stateStr == "7"){
      passIn[countIn] = '7';
    }
    else if (stateStr == "8"){
      passIn[countIn] = '8';
    }
    else if (stateStr == "9"){
      passIn[countIn] = '9';
    }
    else if (stateStr == "0"){
      passIn[countIn] = '0';
    }
    else if (stateStr == "*"){
      Serial.println("Clear");
      countIn = -1;
    }
    else if (stateStr == "#"){
      Serial.println("Enter");
      checkPassword();
      if(check = 0){
        microgear.chat(TargetWeb, "wrong");
      }else if(check){
        Serial.println("Trueeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
      }
      
      for(int i = 0; i<5; i++){
        passIn[i] = '*';
      } 
      
    }
    else if (stateStr == "R"){
      Serial.println("Reset Password");
      countIn = -1;
  
      //reset password
      resetPassword();
    }
    else if (stateStr == "F"){
      Serial.println("Forget Password");
      countIn = -1;
      
      //Send to line noti    
      Line_Notify(pw);    
      Serial.println("Send to Line Noti");
    }
    Serial.print("Count In: ");   Serial.println(countIn);
    Serial.print("Pass In: ");   Serial.println(passIn);
    countIn++;
  }  
  
  Serial.println(passIn);
  
}

void resetPassword(){
  resetPass = 1;  
}

void checkPassword(){
  int n = 0;
  if(countIn == limit || countRe == limit){
    for(int i = 0; i<limit; i++){
      if(passIn[i] == pw[i] || passRe[i] == pw[i]){
        n++;
    }
  }
  Serial.print("n: "); 
  Serial.println(n); 
  Serial.print("count: "); 
  Serial.println(countIn);
  
  if(limit == 0 && countIn != 0){
    Serial.println("XXXXXXXXXXXXXXX Incorrect 1 XXXXXXXXXXXXXXXXXX");
    check = 0;
  }else if(n == limit){
    Serial.println("--------------- Correct!! ------------------");
    check = 1;
    Serial.print("Check(T): ");
    Serial.println(check);
    if(resetPass == 0){
      openDoor();
    }
  }else{
    Serial.println("XXXXXXXXXXXXXXX Incorrect 2 XXXXXXXXXXXXXXXXXX");
    check = 0;
  }
  
  countIn = -1;
  countRe = -1;
}
}

void onConnected(char *attribute, uint8_t* msg, unsigned int msglen)
{
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);
}

void recordTime(){
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

void Line_Notify(String message) {
  WiFiClientSecure client;
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("Connection Line failed");
    return;
  }
  
  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message;
  client.print(req);
  delay(20);
  
  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
    break;
    }
  }
}

void openDoor(){
  digitalWrite(lock, LOW);
  delay(3000);
  digitalWrite(lock, HIGH);
}
