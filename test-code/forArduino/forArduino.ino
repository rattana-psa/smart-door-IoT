#define lock 9
#include <Servo.h>
Servo myservo; 

#include <SPI.h>
#include <SD.h>

#include "Keypad.h"
#include <LiquidCrystal.h>

LiquidCrystal lcd(22, 24, 26, 28, 30, 32);

File myFile; // สร้างออฟเจก File สำหรับจัดการข้อมูล
const int chipSelect = 4;

int pos = 0; 

int button = 8;
int led = 13 ;
int clr = 0;
int tme = 0;
int buz = 47;
int bell = 8;   //ออด
int unlock = 7; //ปลดจากในบ้าน

int r = 49;
int g = 51;
int b = 53;

int limit = 5;
char pass[9] = { '*', '*', '*', '*', '*' };
char password[9] = { '0', '0', '0', '0', '0' };
int count = 0;
int countChangePassword = 0;
int toReset = 2;
boolean check;
char key;
int cur = 0;
int curs = 3;
 
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {14, 15, 16, 17}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {18, 19, 20}; //connect to the column pinouts of the keypad
 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
 
void setup()
{
  myservo.attach(4); // GPIO 4 (D2)
  Serial.begin(9600);  
  pinMode(button, INPUT);
  pinMode(bell, INPUT);
  pinMode(unlock, INPUT);
  pinMode(led, OUTPUT);  
  pinMode(lock, OUTPUT);
  pinMode(r, OUTPUT); 
  pinMode(g, OUTPUT); 
  pinMode(b, OUTPUT);
  
  Serial.println("WELCOME! :)");
  lcd.begin(16, 2); 
  tone(buz, 0);
  
  
}

void loop()
{   
  int un = digitalRead(unlock);
  digitalWrite(lock, HIGH); 
  tone(buz, 0);
  check = false;  
  lcd.setCursor(0, 0);
  lcd.print("WELCOME! :)"); 
  key = keypad.getKey();
  if(key != NO_KEY)
  {
    //Reset Password
    if(toReset == 2 && key == '*'){
      Serial.println("******* Reset Password *******");
      Serial.print("Enter Password : ");
      lcd.clear();
      lcd.print("Reset Password");  
      lcd.setCursor(0, 1);  
      lcd.print("Old PW: ");  
      
      //เช็ครหัสเก่าก่อน  
      Serial.println("Old Password"); 
      if(limit != 0){
        int a = 7;
        for(int i=0; i<=limit; i++){
          inputKey();        
          if(key == '#' && i<limit)   break;
          inPass(key);
          count = enterPassword(key, i);           
          lcd.setCursor(a, 1);  
          lcd.print("*"); 
          a++;
          if(key == '*'){
            delRst();
            i = -1;
            a = 7;
          }
        }
      }else{
        inputKey();
        if(key == '#'){          
          check = true;
        }
        
        inputKey(); 
        Serial.print(key);
        int a = 3;
        while(key != '#'){  
          inputKey();       
          Serial.print(key);    
          lcd.setCursor(a, 1);  
          lcd.print("*"); 
          a++;
          if(key == '*'){
            delRst();
            a = 3;
            check = true;
          }
        }      
      }    
      
      //ถ้ารหัสถูกให้รีเซ็ตรหัส
      if(key == '#'){
        beep();
        lcd.clear(); 
        if(check == true ){   
          trueSound();           
          lcd.print("Password Correct"); 
          delay(1500);
          showPassword();  
          
          //รหัสกี่ตัว limit
          Serial.print("How many codes? [<9] : ");   
          lcd.clear();
          lcd.print("How Many Codes?");
          lcd.setCursor(0, 1);
          lcd.print("[Max9]:");
          setLimit();
          Serial.println(limit);
          lcd.setCursor(8, 1);
          lcd.print(limit);
          delay(1500);
          lcd.clear();
          
          //ตั้งรหัส for ตามจน.limit
          if(limit != 0){
            Serial.println("Reset Password");
            Serial.println("Enter New Password");
            lcd.print("New Password:");     
            int x = 0;     
            for(int i=0; i<limit; i++){
              char k = resetPassword(i);       
              Serial.print("k : ");      
              Serial.println(k);             
              if(k == '*'){
                i = -1;
                Serial.println("Reeeee");
              }              
            }
            cur = 0;
          }
          delay(2000);
          Serial.println("\nReset Complete!\nNew ");
          lcd.clear();
          lcd.print("Reset Complete!");
          delay(1500);
          lcd.clear();
          resetSound();
          showPassword();
                  
        } else {
          lcdIncr(); 
          delay(1000);
          lcd.clear();       
        }
        count = 0;
      }
    }
  }else if ( key != NO_KEY ){ //มีคนใส่รหัส
      lcd.setCursor(0, 1);
      lcd.print("PW:");
      if(key != '#' && key != '*'){
        for(int i=3; i<=curs; i++){
          lcd.setCursor(i, 1);
          lcd.print("*");
        }
        inPass(key);   
        curs++;
        toReset = 3;
      }else if(key == '#'){
        toReset = 2;
        curs = 3;
      }else{
        count = 0;
        toReset = 3;
        curs = 3;
        lcd.clear();
      }
      count = enterPassword(key, count);    
          
      //ประตูเปิด
      if(key == '#'){
        if(check){     
          Serial.println("Door Open!!!");        
          lcd.clear();
          lcd.print("Password Correct!");
          lcd.setCursor(0, 1);
          lcd.print("Door Open!!!");
          openDoor();
          delay(3000);
          trueSound();        
          lcd.clear();
        }else{
          falseSound();
          lcd.clear();
          lcdIncr();
          delay(1000);
          lcd.clear();
          count = 0;
        } 
      }     
    }else if(un == 1){      //เปิดประตูจากด้านใน    
      Serial.println("Unlock");
      digitalWrite(lock, LOW); //At this point the relay turns on
      delay(2000); 
      digitalWrite(lock, HIGH); //turn off the relay again
    }else{
      Serial.println("*********RUN*******");
    }  
    Serial.println(un);
  }

//LCD Reset
void delRst(){
  lcd.clear(); 
  lcd.print("Reset Password");  
  lcd.setCursor(0, 1);  
  lcd.print("PW: ");   
}

//LCD PW Incorrect
void lcdIncr(){
  lcd.print("Password Incorrect"); 
  delay(1000);
  for(int i=0; i<2; i++){
    lcd.scrollDisplayLeft();
    delay(200);
  }
  delay(2000);
}

//รับข้อมูลจาก Keypad
void inputKey(){
  key = keypad.getKey();
  while(key == NO_KEY){
    key = keypad.getKey();
  }
  beep();
}

void trueSound(){
  tone(buz, 1500);
  delay(500);
  tone(buz, 0);
}

void falseSound(){
  tone(buz, 300);
  delay(500);
  tone(buz, 0);
}

void resetSound(){
  tone(buz, 1000);
  delay(300);
  tone(buz, 0);
  delay(300);
  tone(buz, 1000);
  delay(300);
  tone(buz, 0);
  delay(300);
}       

//รับรหัส
void inPass(char key){
    Serial.print(key);    
}

//แสดงรหัส
void showPassword(){
  Serial.print("Password : ");
  for(int i=0; i<limit; i++){
    Serial.print(password[i]);      
  }
  Serial.println();
}

//ตรวจรหัส
boolean checkPassword( ){
  int n = 0;
  if(count == limit){
    for(int i = 0; i<limit; i++){
      if(pass[i] == password[i]){
        n++;
      }
    }
  }
  Serial.print("n: "); 
  Serial.println(n); 
  Serial.print("count: "); 
  Serial.println(count);
  
  if(limit == 0 && count != 0){
    Serial.println("XXXXXXXXXXXXXXX Incorrect XXXXXXXXXXXXXXXXXX");
    return false;
  }else if(n == limit){
    Serial.println("--------------- Correct!! ------------------");
    return true;
  }else{
    Serial.println("XXXXXXXXXXXXXXX Incorrect XXXXXXXXXXXXXXXXXX");
    return false;
  }
}

//ตั้งรหัส
char resetPassword(int count){  
  inputKey(); 
   
  lcd.setCursor(cur, 1);
  lcd.print(key);
  cur++;
  Serial.print(key);
  switch(key){
      case '1': 
        password[count] = '1'; 
        break;
      case '2': 
        password[count] = '2'; 
        break;
      case '3': 
        password[count] = '3'; 
        break;
      case '4': 
        password[count] = '4'; 
        break;
      case '5': 
        password[count] = '5'; 
        break;
      case '6': 
        password[count] = '6'; 
        break;
      case '7': 
        password[count] = '7';
        break;
      case '8': 
        password[count] = '8'; 
        break;
      case '9': 
        password[count] = '9'; 
        break;
      case '0': 
        password[count] = '0'; 
        break;
      case '*': //Delete รหัสเพื่อใส่ใหม่
        Serial.println("Deleteeeeeee"); 
        cur = 0;  
        lcd.clear();
        lcd.print("New Password:");
        break;
    }
    return key;
}

//รับรหัส
int enterPassword(char Key, int Count){
  switch(Key){
      case '1': 
        pass[Count] = '1'; 
        Count++; 
        break;
      case '2': 
        pass[Count] = '2'; 
        Count++; 
        break;
      case '3': 
        pass[Count] = '3'; 
        Count++; 
        break;
      case '4': 
        pass[Count] = '4'; 
        Count++; 
        break;
      case '5': 
        pass[Count] = '5'; 
        Count++; 
        break;
      case '6': 
        pass[Count] = '6'; 
        Count++; 
        break;
      case '7': 
        pass[Count] = '7';
        Count++; 
        break;
      case '8': 
        pass[Count] = '8'; 
        Count++; 
        break;
      case '9': 
        pass[Count] = '9'; 
        Count++; 
        break;
      case '0': 
        pass[Count] = '0'; 
        Count++; 
        break;
      case '*': //Delete รหัสเพื่อใส่ใหม่
        Count = 0;      
        Serial.println("Delete"); 
        break;
      case '#': //ตรวจรหัส
        check = checkPassword();  
        Count = 0;      
        break;
    }  
    return Count;
}

//ตั้งความยาวรหัส
void setLimit(){
  inputKey();
  switch(key){
      case '1': 
        limit = 1;
        break;
      case '2': 
        limit = 2; 
        break;
      case '3': 
        limit = 3;  
        break;
      case '4': 
        limit = 4; 
        break;
      case '5': 
        limit = 5; 
        break;
      case '6': 
        limit = 6; 
        break;
      case '7': 
        limit = 7; 
        break;
      case '8': 
        limit = 8; 
        break;
      case '9': 
        limit = 9; 
        break;
      case '0': 
        limit = 0; 
        break;
      default:
        setLimit();
        break;
  }  
}

void openDoor(){
  digitalWrite(lock, LOW);  //OPEN
}

void beep(){
  tone(buz, 700);
  delay(50);
  tone(buz, 0);
}

/*
void keyLED(){
  analogWrite(r, LOW);
  analogWrite(g, HIGH);
  analogWrite(b, LOW);
}

void lowLED(){
  analogWrite(r, LOW);
  analogWrite(g, LOW);
  analogWrite(b, LOW);   
}

void correctLED(){
  analogWrite(r, HIGH);
  analogWrite(g, HIGH);
  analogWrite(b, HIGH);   
  delay(3000);
}
*/
