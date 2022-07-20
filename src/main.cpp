#include <ArduinoOTA.h>
#include <WiFi.h>
#include <time.h>
#include <Espalexa.h>
Espalexa espalexa;

// WiFi設定pushするとき消す
const char *WIFI_SSID = "eoRT-1b3cdbf-g";
const char *WIFI_PASS = "330e5c2e64e339";



// 時刻設定
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 32400;
const int  daylightOffset_sec = 0;

// RGBWピン
// R=33 G=27 B=13 W=14
int mode = 0;

//チャンネル設定
const int redChannel = 1;
const int greenChannel = 2;
const int blueChannel = 3;
const int whiteChannel = 4;

// 割り込み処理用ピン
const int whiteUpButton = 2;
const int whiteDownButton = 15;
// チャタリング対策
bool switchFlg = false;

void whiteUp(void);
void whiteDown(void);
//RGBW変数代入
int r = 0;
int g = 0;
int b = 0;
int w = 0;

// 照明光度調整用
int i = 0;

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
  ledcAttachPin(33, whiteChannel);
  ledcAttachPin(27, blueChannel);
  ledcAttachPin(13, greenChannel);
  ledcAttachPin(14, redChannel);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi接続中");
  // WiFi接続を確認
  while (WiFi.status() != WL_CONNECTED)
  {
    colorSet(0, 0, 0, 5);
    Serial.print(".");
    delay(150);
    colorSet(0, 0, 0, 0);
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
  espalexa.addDevice("部屋", firstLightChanged); 
  espalexa.begin();

   //時間情報取得
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);


  pinMode(whiteUpButton, INPUT_PULLUP);
  attachInterrupt(whiteUpButton, whiteUp, RISING);
  pinMode(whiteDownButton, INPUT_PULLUP);
  attachInterrupt(whiteDownButton, whiteDown, RISING);

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
  
  // チャタリング対策
  if(switchFlg == true){
      w = w + 20;
      w = min(255 ,w);
      colorSet(r, g, b, w);
      putColor(r, g, b, w);
      delay(50);
      switchFlg = false;
    }

  colorSet(r, g, b, w);
  if(mode == 1){
    switch (timeInfo.tm_hour){
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        chengeColor(100, 100, 0, 0);
        break;
      case 6:
        chengeColor(100, 100, 100, 100);
        break;
      case 7:
        chengeColor(120, 120, 120, 120);
        break;
      case 8:
        chengeColor(150, 150, 150, 150);
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15:
      case 16:
        chengeColor(100, 100, 100, 200);
        break;
      case 17:
        chengeColor(100, 100, 100, 50);
        break;
      case 18:
        chengeColor(100, 100, 80, 50);
        break;
      case 19:
        chengeColor(100,100,45, 50);
        break;
      case 20: 
      case 21:
      case 22:
      case 23:
        chengeColor(100,100,0, 0);
        break;
      default:
        break;
    } 
  }
  else{
    switch (timeInfo.tm_hour){
      case 6:
        if ((timeInfo.tm_min >= 0) && (timeInfo.tm_min <=10)){
            
          while(r <= 130){
            r++;
            g++;
            b++;
            w++;
            colorSet(r, g, b, w);
            delay(10);
          }
          while(r >= 1){
            r--;
            g--;
            b--;
            w--;
            colorSet(r, g, b, w);
            delay(10);
          }
          
        } 
    }
  }
}

void colorSet(int red, int green, int blue, int white){
  ledcWrite(redChannel, red);
  ledcWrite(greenChannel, green);
  ledcWrite(blueChannel, blue);
  ledcWrite(whiteChannel, white);
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
    else if(w >= white + 1){
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

// 割り込み処理1
void IRAM_ATTR whiteUp(void){
  switchFlg = true;
  mode = 0;
  // w = w + 20;
  // w = min(255 ,w);
  // colorSet(r, g, b, w);
  // putColor(r, g, b, w);
}
// 割り込み処理2
void whiteDown(void){
  switchFlg = true;
  mode = 0;
  w = w -20;
  w = max(0 , w);
  colorSet(0, 0, 0, 0);
  putColor(r, g, b, w);
}

void firstLightChanged(uint8_t brightness){
Serial.println(brightness);
  if (brightness == 255) {
    mode = 1;
  }
  else if(brightness == 27){
    mode = 0;
    chengeColor(0, 0, 0, 255);
  }
  
  else if(brightness == 204){//明るくする
    mode = 0;
    for(i = 0; i <= 29; i++){
      w++;
      colorSet(r, g, b, w);
      delay(50);
    }
  }
  else if(brightness == 230){//暗くする
    mode = 0;
    chengeColor(0, 0, 0,90);
  }
  // else if(brightness == 103){
  //   mode = 0;
  //   chengeColor(0, 200, 0);
  // }
  // else if(brightness == 129){
  //   mode = 0;
  //   chengeColor(0, 0, 200);
  // }
  else  {
    mode = 0;
    while((r >= 1) || (g >= 1) || (b >= 1) || (w >= 1)){
      r--;
      g--;
      b--;
      w--;
   
      r = max(0, r);
      g = max(0, g);
      b = max(0, b);
      w = max(0, w);

      colorSet(r, g, b, w);
    
      delay(15);
      Serial.println(brightness);
    }
  }            

}