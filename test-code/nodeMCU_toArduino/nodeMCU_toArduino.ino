#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>

SoftwareSerial NodeSerial(D2, D3); // RX | TX

void setup() {
pinMode(D2, INPUT);
pinMode(D3, OUTPUT);
Serial.begin(115200);
NodeSerial.begin(4800);
}
void loop() {
NodeSerial.print(10);
NodeSerial.print("\n");
delay(100);
}
