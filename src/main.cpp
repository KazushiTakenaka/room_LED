#include <ArduinoOTA.h>
#include <WiFi.h>
#include <time.h>
#include <Espalexa.h>
Espalexa espalexa;

// WiFi設定pushするとき消す



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

//シリアル出力RGB
void putColor(int red, int green, int blue, int white);
void setup() {
  Serial.begin(115200);
  // LED_PWM出力チャンネルをGPIOに割り当てる(チャンネル,周波数,PWMの範囲)
  ledcSetup(redChannel, 12800, 8);
  ledcSetup(greenChannel, 12800, 8);
  ledcSetup(blueChannel, 12800, 8);
  ledcSetup(whiteChannel, 12800, 8);

  // ↑同じような感じ？よくわからん(ピン番号,チャンネル)
  ledcAttachPin(33, redChannel);
  ledcAttachPin(27, greenChannel);
  ledcAttachPin(13, blueChannel);
  ledcAttachPin(14, whiteChannel);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi接続中");
  // WiFi接続を確認
  while (WiFi.status() != WL_CONNECTED)
  {
    chengeColor(0, 0, 0, 5);
    Serial.print(".");
    delay(150);
    chengeColor(5, 0, 0, 0);
    delay(150);
  }

  //arduinoOTAの設定など(コピペ内容わからん)
  ArduinoOTA.setHostname("roomESP32");//ここの名前を変更しないといけない+iniファイルも変更する
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  ArduinoOTA.begin();

  // アレクサに追加
  espalexa.addDevice("電気", firstLightChanged); 
  espalexa.begin();

   //時間情報取得
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // //時刻格納作成
  // struct tm timeInfo; //時刻を格納するオブジェクト
  // char s[20]; //文字列格納用
}
//時刻格納作成
struct tm timeInfo; //時刻を格納するオブジェクト
char s[20]; //文字列格納用

void loop() {
  ArduinoOTA.handle();
  getLocalTime(&timeInfo);
  sprintf(s, "%02d%02d", timeInfo.tm_hour, timeInfo.tm_min);
  espalexa.loop();
  delay(1);

  chengeColor(255, 255, 255, 255);

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
    putColor(r, g, b, w);
  }
}

void putColor(int red, int green, int blue, int white){
  Serial.print("R=");
  Serial.print(red);
  Serial.print(" G=");
  Serial.print(green);
  Serial.print(" B=");
  Serial.print(blue);
  Serial.print(" W=");
  Serial.println(white);
}

void firstLightChanged(uint8_t brightness){



}
