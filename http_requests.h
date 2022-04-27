// post request
bool postRequest(String json, String url) {
  if(WiFi.status() == WL_CONNECTED) {
    httpClient.begin(wifiClient,url);
    httpClient.addHeader("Content-Type","application/json");
    int statusCode = httpClient.POST(json);
    Serial.println(statusCode);
    httpClient.end();
    if(statusCode >= 200) {
      return true;
    }
    return false;
  }
}
// get request
DynamicJsonDocument getRequest(String url, int capacity) {
  int docCapacity = JSON_OBJECT_SIZE(capacity);
  DynamicJsonDocument doc(docCapacity);
  if(WiFi.status() == WL_CONNECTED) {
    httpClient.begin(wifiClient,url);
    int statusCode = httpClient.GET();
    Serial.println(statusCode);
    if(statusCode == 200) {
      String json = httpClient.getString();
      DeserializationError err = deserializeJson(doc,json);
      if (err) {
        Serial.println(err.f_str());
      }
    }
  }
  return doc;
}
// post sanitizer level
void postSanitizerLevel() {
  const int capacity = JSON_OBJECT_SIZE(1);
  StaticJsonDocument<capacity> doc;
  if(sanitizerReadCount == 0) {
    return;
  }
  float averageSanitizerLevel = totalSanitizerLevel / sanitizerReadCount;
  totalSanitizerLevel = 0;
  sanitizerReadCount = 0;
  if(averageSanitizerLevel != NULL) {
    doc["percentage"] = averageSanitizerLevel;
    String levelJson;
    serializeJson(doc,levelJson);
    postRequest(levelJson,SL_URL);    
  }
}
// post notification
void postUserNotification() {
  if(!lowPriorityPosted || !highPriorityPosted) {
    const int capacity = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;
    doc["percentage"] = sanitizerLevel;
    if(sanitizerLevel <= 15) {
      doc["priority"] = 1;
      highPriorityPosted = true;
    } else {
      doc["priority"] = 0;
      lowPriorityPosted = true;
    }
    String notificationJson;
    serializeJson(doc,notificationJson);
    postRequest(notificationJson,NT_URL);
  }
}
// get daily usage
void getDailyUsage() {
  DynamicJsonDocument doc = getRequest(DU_URL,2);
  if (doc == NULL) {
    return;
  } else {
    int usage = doc["dailyUsage"].as<int>();
    usageCount = usage; 
  }
}
// post daily usage
void postDailyUsage() {
  bool result = postRequest("",DU_URL);
  if(result) {
    getUsageCount = true;
    postUsageCount = false;
  }
}
// code on core 0
void httpRequests(void* parameter) {
  // infinite loop
  for(;;) {
    if (usageCount == -1 || getUsageCount) {
      if(WiFi.status() == WL_CONNECTED) {
        getDailyUsage();
        getUsageCount = false;
      }
    }
    if(postUsageCount) {
      postDailyUsage();
    }
    if(postNotification) {
      postUserNotification();
      postNotification = false;
    }
    if(staON) {
      if(WiFi.status() != WL_CONNECTED && (millis() >= nextWifiConnect) && connectToWifi) {
        WiFi.disconnect();
        WiFi.reconnect();
        nextWifiConnect = millis() + WIFI_TIMEOUT;
      }
      if(millis() >= nextSanitizerPost) {
        postSanitizerLevel();
        nextSanitizerPost = millis() + sanitizerPostInterval;
      } 
    }
    delay(2); // prevent inactivity crash
  }
}
