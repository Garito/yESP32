# yESP32
This library provides a base class for your ESP32 with the filesystem with SPIFSS, Wifi both as an access point and connected to an existing network and updates "over the air"

## Base class
As a base class, you should create a subclass with the features that your board will provide

### How to subclass yESP32
Lets put an example by creating a webserver (with [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer)) that will serve only one page with a "hello world" message on it

#### Machine.h
First we should create a headers file:
```cpp
#include <ESPAsyncWebServer.h>

#include "yESP32.h"

class Machine : public yESP32 {
  private:
    AsyncWebServer *_webserver;
    void setupRoutes();
    String tmplProcessor(const String& var);
  public:
    Machine(bool fs, bool ap, String ssid, String password, bool OTA) : yESP32(fs, ap, ssid, password, OTA) {}
    void init(uint16_t port);
};

```
As you can see, the private section of the subclass has `_webserver`, `setupRoutes` and `tmplProcessor`. The first is the webserver, the second is the function that will set the routes for the webserver (in our case only one) and the last is the processor function if your want to use templates (see the [webserver documentation](https://github.com/me-no-dev/ESPAsyncWebServer#respond-with-content-using-a-callback-containing-templates))

The public section defines the constructor that will use the base's one and `init` to start the webserver, setup the routes (by using the `setupRoutes`) and start the webserver

#### Machine.cpp
Next step is to create the code for the new subclass
```cpp
#include <ESPAsyncWebServer.h>

#include "machine.h"

void Machine::init(uint16_t port) {
  Serial.println("Initializing...");
  _webserver = new AsyncWebServer(port);

  setupRoutes();
  _webserver->begin();
  Serial.println("Initialized...");
}

String Machine::tmplProcessor(const String &var) {
  if(var == "My_IP") {
    return myIP().toString();
  }
  return String();
}

void Machine::setupRoutes() {
  _webserver->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    Serial.println("Serving index");
    request->send(SPIFFS, "/site/index.html", String(), false, [this](const String &var) { return this->tmplProcessor(var); });
  });

  _webserver->serveStatic("/", SPIFFS, "/site/");

  _webserver->onNotFound([](AsyncWebServerRequest *request) {
    Serial.println(request->url());
    Serial.println("not found");
    request->send(404);
  });
}
```
In this example, `init` creates the webserver, setup the routes with the function that the subclass provides and starts the webserver

`tmplProcessor` defines a variable `My_IP` that will be parsed on your templates. You can add as much variables as you need or even define and use more than one processor. Please note that since webserver's `on` processor needs to be an static function and the example uses the machine's IP address, it should be passed inside a lambda function with `this` binded

`setupRoutes` defines the routes of your server. In this example, only one routes has been defined `/` that will load the file on `/site/index.html` and will parse it with the defined processor

As an example, this function defines to serve static files from `/site/` folder of your ESP32 filesystem and shows how to process `not found` errors

#### Data folder
As you have noticed in the example, we are using the webserver's files (html, js, css and any other) as static files in the ESP32 filesystem. In this example only index.html will be described sinces from here on is not different to any other webserver
```html {.line-numbers}
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Your webserver</title>
    <link rel="stylesheet" href="/libs/bulma.min.css">
  </head>
  <body>
  <section class="hero is-fullheight has-text-centered">
    <div class="hero-body">
      <div class="container">
        <h1 class="title">Hello world from '%My_IP%</h1>
      </div>
    </div>
  </section>
  </body>
</html>
```
As you can see, the example uses [Bulma](https://bulma.io/) for the styles

You should expect to see a webpage with `Hello world from <the machine's IP>`

## Use the subclass
To use the code defined you could do something around this:
```cpp
#include "machine.h"

String APName = "<Put here the name of the access point>";
String APPassword = "<Put here the password of the access point>";
Machine machine(true, false, APName, APPassword, true);

void setup() {
  Serial.begin(115200);
  Serial.println("Machine setup...");
  machine.setup();
  Serial.println("Machine init...");
  machine.init(80);
  Serial.println("Setup finished...");
}

void loop() {
  machine.loop();

  delay(500);
}
```
The ESP32 can be used as access point or connected to another network

In any case, `APName` will be the name of the access point for the ESP32 if you decide not to connect it to a previous network or the name of the network you want to connect it, `APPassword` will be the password

In this case, the example inits the machine with filesystem, to another network and with OTA

Then in the setup section you need to run its `setup` and `init` the webserver (in this example will be on port 80)

As the example uses OTA, you must run the `loop` function on the machine that will run the update

If you need to define more functionality on the loop of your program, override `loop` in your subclass

## Contribute
Feel free to comment and contribute on this library

## Donate
If you can't contribute directly to this library but you still want to help feel free to send some cryptos

BTC: 1GtKxwZGR65ar9V8xafxhMiniZyqXej2GC

ETH: 0x01bd478b8C07633D2f4E58AC553f72CE4E590d56

LTC: LYUzrFX6ck5uMhw5VqcD9piQHnX7oeSLdh

XMR: 49stcvbfjEkWLjb6mdG21zMJ3uRrLmN3bazGQ8cHjjsVHYYyY61N6P7emCXhpsvB2Vc8Uuz2FA1Qk6hkE8e4ADmJQQ64eyT

ADA: DdzFFzCqrhsoUF5UjGGAYUayV5uNCJZ17PJn9V8X9MTQ26m2wDVycme42gufKufPNWMazfJLg8RKHpc1iFvn6j8BTJjaozGtLPzCDx5t

NEM: NDGYO6X3NTD6CX3V7MCCYKQPBIOYGZRXEKDLCDW2