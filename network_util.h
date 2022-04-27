#include <AsyncElegantOTA.h>

// Setup WIFI as Access Point
void initAP() {
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Setting up Access Point");
  display.display();
  // setting up access point
  WiFi.softAP(SERVER_SSID,SERVER_PWD);
  IPAddress IP = WiFi.softAPIP();
  Serial.println(IP);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connect to: ");
  display.setCursor(0,9);
  display.println("ESP32 Server");
  display.setCursor(0,18);
  display.println("Go to: ");
  display.setCursor(0,27);
  display.print("http://");
  display.print(IP);
  display.println(":" + String(SERVER_PORT));
  display.display();
  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("Yes OTA Works");
}
// Setup WIFI as Station
void initSTA() {
  unsigned long connectionTimeout = millis() + WIFI_TIMEOUT;
  server.end();
  WiFi.mode(WIFI_STA);
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to");
  ssid = preferences.getString("ssid","");
  password = preferences.getString("password","");
  display.setCursor(0,9);
  display.print("WIFI: "+ ssid);
  display.display();
  WiFi.begin(ssid.c_str(),password.c_str());
  while(WiFi.status() != WL_CONNECTED && (millis() <= connectionTimeout)) {
    if(digitalRead(APPIN)) { break; }
    if(WiFi.status() == WL_CONNECT_FAILED) { break; }
  }
}
// set server routes
void setServerRoutes() {
  server.on("/",HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200,"text/html",index_html);
  });
  server.on("/done",HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(201,"text/html",complete_html); 
  });
  server.on("/connect",HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid = request->getParam("ssid",true)->value();
    String password = request->getParam("password",true)->value();
    String email = request->getParam("email",true)->value();
    // saves ssid and password
    preferences.putString("ssid",ssid);
    preferences.putString("password",password);
    preferences.putString("email", email);
    request->redirect("/done");
  }); 
}
