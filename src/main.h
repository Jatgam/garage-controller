#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266TrueRandom.h>

#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

#include "global.h"
#include "door.h"
#include "temperature.h"

//#include "ModuleVersion.h"

#define JATGAM_MODULE_VERSION "0.1.0"

#define JATGAM_MODULE "garage-controller"

#define AP_AUTOCONFIG_SSID "GarageController"
#define AP_AUTOCONFIG_PASSWORD "jatgam42"

class Garage {
public:
    Garage();
    Door* door1;
    Door* door2;
    Temperature* temp;
private:
protected:
};

#endif
