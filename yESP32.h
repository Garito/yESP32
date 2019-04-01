/*
yESP32.h - ESP32 class with Filesystem, Wifi & OTA bootstraped
*/

#ifndef yESP32_h
#define yESP32_h

#include <Arduino.h>

class yESP32 {
  public:
    yESP32(bool fs, bool ap, String ssid, String password, bool OTA);
    void setup();
    bool hasFS();
    void loop();
    IPAddress myIP();
  private:
    bool _fs;
    bool _ap;
    String _ssid;
    String _password;
    IPAddress _IP;
    bool _OTA;

    void setupFS();
    void setupWifi();
    void setupOTA();
};

#endif
