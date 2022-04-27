#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Preferences.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include "ESPAsyncWebServer.h"
#include "index_page.h"
#include "complete.h"

#define APPIN 18
#define STAPIN 19
#define TRIG 27
#define ECHO 34
#define SOUND_SPEED 0.034 // cm/us
#define MTR_PIN 5
#define RLED 14
#define GLED 13
#define LDR_PIN 35
#define LSR_PIN 16
#define SERVER_PORT 80
#define WIFI_TIMEOUT 10000L
#define LDR_THR 3900
#define SL_URL "http://192.168.217.76:3000/api/sanitizer-level"
#define NT_URL "http://192.168.217.76:3000/api/notification"
#define DU_URL "http://192.168.217.76:3000/api/usage-count"
#define I2C_ADDRESS 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SERVER_SSID "AHSM"
#define SERVER_PWD "123456"

String ssid, password, email;
bool staON = true;
bool apON = false;
// initialize sanitizer level read & post varaibles
unsigned long sanitizerReadInterval = 1000L;
unsigned long nextSanitizerRead = 0L;
float totalSanitizerLevel = 0.0;
float sanitizerLevel = 33.5;
byte sanitizerReadCount = 0;
unsigned long sanitizerPostInterval = 10000L;
unsigned long nextSanitizerPost;
// daily usage
int usageCount = -1;
bool postUsageCount = false;
bool getUsageCount = false;
bool levelLow = false;
// dispenser control
bool isDispensing = false;
unsigned long dispenseInterval = 1000L;
unsigned long dispenseTimeout = 0L;
// wifi
unsigned long nextWifiConnect = 0L;
bool connectToWifi = true;
// oled status data
byte topHorizontalY,verticalY = 14;
byte horizontalX,verticalX = 0;
byte verticalLength = 15;
byte horizontalLength = 70;
byte bottomHorizontalY = verticalLength + verticalY;
byte wifiTextY = bottomHorizontalY + 8;
// notification
bool postNotification = false;
bool lowPriorityPosted = false;
bool highPriorityPosted = false;

AsyncWebServer server(SERVER_PORT); // setting up server on port 80
Preferences preferences;
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire, OLED_RESET);
TaskHandle_t HttpRequests;
WiFiClient wifiClient;
HTTPClient httpClient;

#include "network_util.h"
#include "http_requests.h"
#include "sensors_util.h"

void setup() {
  Serial.begin(115200);
  // set up io pins
  pinMode(APPIN,OUTPUT);
  pinMode(STAPIN,OUTPUT);
  pinMode(RLED,OUTPUT);
  pinMode(GLED,OUTPUT);
  pinMode(LDR_PIN,INPUT);
  pinMode(LSR_PIN,OUTPUT);
  pinMode(MTR_PIN,OUTPUT);
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  // init lcd
  display.begin(I2C_ADDRESS,true);
  display.display();
  display.clearDisplay();
  // set font & color
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.println("**AHSM**");
  display.setCursor(0,9);
  display.println("Initialising");
  display.display();
  // set up second core (0)
  nextWifiConnect = millis() + WIFI_TIMEOUT;
  nextSanitizerPost = millis() + sanitizerPostInterval;
  xTaskCreatePinnedToCore(
    &httpRequests,
    "HTTP Requests",
    3000,
    NULL,
    1,
    &HttpRequests,
    0
  );
  delay(2000); // ensure second core begins
  // sets up credentials preference in read/write mode
  preferences.begin("credentials",false);
  setServerRoutes();
  initSTA(); // start wifi in station mode
  toggleLaser(HIGH);
  delay(10);  
}

void loop() {
  // check which mode to switch to (config || operation)
  int apValue = digitalRead(APPIN);
  int staValue = digitalRead(STAPIN);
  if(staValue == 1 && !staON) {
    initSTA();
    staON = true;
    apON = false;
    toggleLaser(HIGH);
  }
  if(apValue == 1 && !apON) {
    initAP(); // start wifi as access point
    staON = false;
    apON = true;
    toggleLaser(LOW);
  }
  // dispenser
  if(!apON) {
    detectHand();
    controlDispenser();
  }
  // read sanitizer value
  if(millis() >= nextSanitizerRead && !apON) {
    readSanitizerLevel();
    totalSanitizerLevel += sanitizerLevel;
    sanitizerReadCount++;
    nextSanitizerRead = millis() + sanitizerReadInterval;
  }
  // print values to lcd.
  if(!apON) {
    showStatusData();
  }
  // idicator LED's
  if(WiFi.status() == WL_CONNECTED) {
    digitalWrite(GLED,HIGH);
  } else {
    digitalWrite(GLED,LOW);    
  }
  if(levelLow) {
    digitalWrite(RLED,HIGH);
  } else {
    digitalWrite(RLED,LOW);
  }
}

void showStatusData(void) {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Sanitizer Level:");
  // draw level container
  display.drawFastVLine(verticalX,verticalY,verticalLength,SH110X_WHITE);
  display.drawFastHLine(horizontalX,verticalY,horizontalLength,SH110X_WHITE);
  display.drawFastHLine(horizontalX,bottomHorizontalY,horizontalLength,SH110X_WHITE);
  drawPercentRect(sanitizerLevel);
  display.setCursor(horizontalLength + 10,((8 + verticalY)/2)+5);
  display.println(String((int)sanitizerLevel) + "%");
  // wifi text
  display.setCursor(0,wifiTextY);
  if (WiFi.status() == WL_CONNECTED) {
    display.print(F("WIFI: Connected")); 
  } else {
    display.print(F("WIFI: Disconnected"));
  }
  display.setCursor(0, wifiTextY + 15);
  if(usageCount == -1) {
    display.println("Usage Today: Loading");
  } else {
    display.println("Usage Today: " + String(usageCount));
  }
  display.display();
}
void drawPercentRect(float percentage) {
  int percentPixelLength = round((percentage/100)*horizontalLength) + 1;
  for(byte x = verticalX + 1; x <= percentPixelLength; x++) {
    for (byte y = verticalY + 1; y <= bottomHorizontalY - 1; y++) {
      display.drawPixel(x,y,SH110X_WHITE);
    }
  }
}
