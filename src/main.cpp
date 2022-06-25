#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <Espalexa.h>
Espalexa espalexa;

// WiFi設定pushするとき消す
const char *WIFI_SSID = "";
const char *WIFI_PASS = "";

// 時刻設定
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 32400;
const int  daylightOffset_sec = 0;

// RGBWピン
// R=33 G=27 B=13 W=14

//チャンネル設定
const int redChannel = 1;
const int greenChannel = 2;
const int blueChannel = 3;
const int whiteChannel = 4;

//RGBW変数代入
int r = 0;
int g = 0;
int b = 0;
int w = 0;

//Alexa設定
void firstLightChanged(uint8_t brightness);

//チャンネルセット
void colorSet(int red, int green, int blue, int white);

//色変更
void chengeColor(int red, int green, int blue, int white);

void setup() {
  
}

void loop() {
  // put your main code here, to run repeatedly:
}

void colorSet(int red, int green, int blue, int white){
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
}

void chengeColor(int red, int green, int blue, int white){
  while(true){
    if(r <= red - 1){
      r++;
    }
    else if(r >= red + 1){
      r--;
    }
    else{
      r = red;
    }

    if(g <= green -1 ){
      g++;
    }
    else if(g >= green + 1){
      g--;
    }
    else{
      g = green;
    }

    if(b <= blue - 1){
      b++;
    }
    else if(b >= blue + 1){
      b--;
    }
    else{
      b = blue;
    }

    if(w <= white -1){
      w++;
    }
    else if(w >= white){
      w--;
    }
    else{
      w = white;
    }

    if((r == red) && (g == green) && (b == blue) && (w == white)){
      break;
    }
    colorSet(r, g, b, w);
    delay(15);
}