// ----------------------------
// Board Version 2.6.3
// ----------------------------

#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <PxMatrix.h>
#define DHTPIN 0            //D5
#define DHTTYPE    DHT11     // DHT 11

// https://github.com/2dom/PxMatrix

#include <YoutubeApi.h>
// https://github.com/witnessmenow/arduino-youtube-api

#include <ArduinoJson.h>
// https://github.com/bblanchon/ArduinoJson

char ssid[]     = "xxxxxxxx";  // your network SSID (name)
char password[] = "xxxxxxxx";  // your network key
#define API_KEY "AIzaSyB83IrbScCZzI3UuQXHwtKycyYp3zHcoa4"  // your google apps API Token (see readme of YT API library)
#define CHANNEL_ID "UClQNMvRPG5lCHfL98pqmgyg" // makes up the url of channel

DHT dht(DHTPIN, DHTTYPE);
Ticker display_ticker;

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

unsigned long api_mtbs = 10000; //mean time between api requests
unsigned long api_lasttime = 10000;   //last time api request has been done

// Pins for LED MATRIX
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_OE 2
#define P_D 12
#define P_E 0
unsigned long ti;
// PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B,P_C);
 PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

// Some standard colors
uint16_t myRED = display.color565(255, 0, 0);
uint16_t myGREEN = display.color565(0, 255, 0);
uint16_t myBLUE = display.color565(0, 0, 255);
uint16_t myWHITE = display.color565(255, 255, 255);
uint16_t myYELLOW = display.color565(255, 255, 0);
uint16_t myCYAN = display.color565(0, 255, 255);
uint16_t myMAGENTA = display.color565(255, 0, 255);
uint16_t myBLACK = display.color565(0, 0, 0);

uint16 myCOLORS[8] = {myRED, myGREEN, myBLUE, myWHITE, myYELLOW, myCYAN, myMAGENTA, myBLACK};

// Converted using the following site: http://www.rinkydinkelectronics.com/t_imageconverter565.php
uint16_t static youTubeBigger[] = {
  0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0010 (16) pixels
  0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0020 (32) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0030 (48) pixels
  0xF800, 0xF800, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0040 (64) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0050 (80) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800,   // 0x0060 (96) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0070 (112) pixels
  0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0080 (128) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0090 (144) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,   // 0x00A0 (160) pixels
  0xFFFF, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00B0 (176) pixels
  0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00C0 (192) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x00D0 (208) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xF800, 0xF800,   // 0x00E0 (224) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF,   // 0x00F0 (240) pixels
  0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0100 (256) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xFFFF, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0110 (272) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0120 (288) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800,   // 0x0130 (304) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0xF800, 0xF800, 0xF800,   // 0x0140 (320) pixels
  0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0xF800, 0x0000, 0x0000,   // 0x0150 (336) pixels
};



// ISR for display refresh
void display_updater()
{

  display.display(30);

}
void setup() {
  Serial.begin(115200);
  dht.begin();
  display.begin(16);
  display.clearDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(2, 0);
  display.print("Connecting");
  display.setTextColor(myMAGENTA);
  display.setCursor(2, 8);
  display.print("to the");
  display.setTextColor(myGREEN);
  display.setCursor(2, 16);
  display.print("WiFi!");

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
 #ifdef ESP8266
  // Required if you are using ESP8266 V2.5 or above
  client.setInsecure();
  #endif
 
  display_ticker.attach(0.004, display_updater);
  api.getChannelStatistics(CHANNEL_ID);
}

void drawYouTube(int x, int y)
{
  int counter = 0;
  for (int yy = 0; yy < 16; yy++)
  {
    for (int xx = 0; xx < 21; xx++)
    { 
      display.drawPixel(xx + x , yy + y, youTubeBigger[counter]);
      counter++;
    }
  }
}

void displayText(String text, int yPos) {
  int16_t  x1, y1;
  uint16_t w, h;
  display.setTextSize(1);
  char charBuf[text.length() + 1];
  text.toCharArray(charBuf, text.length() + 1);
  display.getTextBounds(charBuf, 0, yPos, &x1, &y1, &w, &h);
  int startingX = 33 - (w / 2);
  if (startingX < 0) {
    display.setTextSize(1);
    display.getTextBounds(charBuf, 0, yPos, &x1, &y1, &w, &h);
    startingX = 33 - (w / 2);
  }
  display.setCursor(startingX, yPos);
  Serial.println(startingX);
  Serial.println(yPos);
  display.print(text);
}

void Garis(){
  //Kiri
  display.drawLine(15,3, 17, 0, myRED);
  display.drawLine(15,3, 15, 12, myRED);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(15,12, 17, 15, myRED);

  display.drawLine(14,3, 16, 0, myWHITE);
  display.drawLine(14,3, 14, 12, myWHITE);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(14,12, 16, 15, myWHITE);
  
  display.drawLine(9,5, 11, 1, myRED);
  display.drawLine(9,5, 9, 9, myRED);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(9,9, 11, 14, myRED);

  display.drawLine(8,5, 10, 2, myWHITE);
  display.drawLine(8,5, 8, 9, myWHITE);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(8,9, 10, 13, myWHITE);

  display.drawLine(4,5, 5, 3, myRED);
  display.drawLine(4,5, 4, 9, myRED);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(4,9, 5, 12, myRED);

  display.drawLine(3,5, 4, 4, myWHITE);
  display.drawLine(3,5, 3, 9, myWHITE);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(3,9, 4, 11, myWHITE);

  //Kanan
  
  display.drawLine(47,3, 45, 0,   myRED);
  display.drawLine(47,3, 47, 12,  myRED);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(47,12, 45, 15, myRED);

  display.drawLine(48,3, 46, 0,  myWHITE);
  display.drawLine(48,3, 48, 12, myWHITE);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(48,12,46, 15, myWHITE);

  display.drawLine(53,5, 51, 1, myRED);
  display.drawLine(53,5, 53, 9, myRED);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(53,9, 51, 14, myRED);

  display.drawLine(54,5, 52, 2, myWHITE);
  display.drawLine(54,5, 54, 9, myWHITE);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(54,9, 52, 13, myWHITE);

  display.drawLine(58,5, 57, 3, myRED);
  display.drawLine(58,5, 58, 9, myRED);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(58,9, 57, 12, myRED);

  display.drawLine(59,5, 58, 4, myWHITE);
  display.drawLine(59,5, 59, 9, myWHITE);//BarisAwal,ColomAwal,BarisAkhir,ColomAkhir
  display.drawLine(59,9, 58, 11, myWHITE);
  
}
void updateScreen1(long subCount) {
  display.clearDisplay();
  Garis();
  drawYouTube(21, 0);
  display.setTextColor(myGREEN);
  displayText("SUBSCIBERS",17);
  display.setTextColor(myWHITE);
  displayText(String(subCount), 25);
  delay(3000);
}
void updateScreen2(long subCount) {
  display.clearDisplay();
  Garis();
  drawYouTube(21, 0);
  display.setTextColor(myGREEN);
  displayText("VIEWERS",17);
  display.setTextColor(myWHITE);
  displayText(String(subCount), 25);
  delay(3000);
}
void updateScreen3(long subCount) {
  display.clearDisplay();
  Garis();
  drawYouTube(21, 0);
  display.setTextColor(myGREEN);
  displayText("VIDEOS",17);
  display.setTextColor(myWHITE);
  displayText(String(subCount), 25);
  delay(3000);
}
void updateScreen4(long subCount) {
  display.clearDisplay();
  Garis();
  drawYouTube(21, 0);
  display.setTextColor(myGREEN);
  displayText("CHANNEL",17);
  display.setTextColor(myWHITE);
  displayText("B_Mz Store",25);
  delay(3000);
}

void updateScreen5(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  display.clearDisplay();
  Garis();
  drawYouTube(21, 0);
  display.setTextColor(myGREEN);
  display.setCursor(6, 17);
  display.print("T:");
  display.print(t);
  display.print(" C");
  display.setTextColor(myWHITE);
  display.setCursor(6, 25);
  display.print("H:");
  display.print(h);
   display.print(" %");
  delay(3000);
}
void displayName(String first, String last)
{
  display.setTextColor(myCYAN);
  displayText(first, 1);
  display.setTextColor(myMAGENTA);
  displayText(last, 17);
}

void loop() {
  
      updateScreen1(api.channelStats.subscriberCount);
      updateScreen2(api.channelStats.viewCount);
      updateScreen3(api.channelStats.videoCount);
      updateScreen4(api.channelStats.videoCount);  
      updateScreen5();
}


