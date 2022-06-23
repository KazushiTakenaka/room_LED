#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <Espalexa.h>
Espalexa espalexa;

// WiFi設定

// 時刻設定
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 32400;
const int  daylightOffset_sec = 0;

void setup() {
  
}

void loop() {
  // put your main code here, to run repeatedly:
}