#include <SPIFFS.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

#include "yESP32.h"

yESP32::yESP32(bool fs, bool ap, String ssid, String password, bool OTA) {
  _fs = fs;
  _ap = ap;
  _OTA = OTA;
  _ssid = ssid;
  _password = password;
}

void yESP32::setup() {
  if(_fs) {
    setupFS();
  }
  setupWifi();
  if(_OTA) {
    setupOTA();
  }
}

bool yESP32::hasFS() {
  return _fs;
}

IPAddress yESP32::myIP() {
  return _IP;
}

void yESP32::setupFS() {
  if(SPIFFS.begin(true)){
    Serial.println("Filesystem ready");
  } else {
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}

void yESP32::setupWifi() {
  if (_ap) {
    WiFi.softAP(_ssid.c_str(), _password.c_str());
    _IP = WiFi.softAPIP();

    Serial.print("Access point IP address: ");
    Serial.println(_IP);
  } else {
    WiFi.begin(_ssid.c_str(), _password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting...");
    }
    _IP = WiFi.localIP();
    Serial.println("IP address: ");
    Serial.println(_IP);
  }
}

void yESP32::setupOTA() {
  bool stopfs = hasFS();
  ArduinoOTA.onStart([stopfs]() {
    Serial.println("OTA Update Start");
    if(stopfs) {
      SPIFFS.end();
    }
    
    // ws.enable(false);
    // ws.textAll("OTA Update Started");
    // ws.closeAll();
    // events.send("Update Start", "ota");
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("OTA Update end");
    // events.send("Update end", "ota");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    char msg[20];
    sprintf(msg, "OTA Progress: %i", (progress / (total / 100)));
    Serial.println(msg);
    // events.send(msg, "ota");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    if(error == OTA_AUTH_ERROR) {
      Serial.println("OTA Auth Failed");
      // events.send("Auth Failed", "ota");
    } else if(error == OTA_BEGIN_ERROR) {
      Serial.println("OTA Begin Failed");
      // events.send("Begin Failed", "ota");
    } else if(error == OTA_CONNECT_ERROR) {
      Serial.println("OTA Connect Failed");
      // events.send("Connect Failed", "ota");
    } else if(error == OTA_RECEIVE_ERROR) {
      Serial.println("OTA Recieve Failed");
      // events.send("Recieve Failed", "ota");
    } else if(error == OTA_END_ERROR) {
      Serial.println("OTA End Failed");
      // events.send("End Failed", "ota");
    }
  });

  ArduinoOTA.begin();
}

virtual void yESP32::loop() {
  ArduinoOTA.handle();
}
