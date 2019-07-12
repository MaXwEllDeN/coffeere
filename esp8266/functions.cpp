#include "functions.h"

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "FS.h"
#include "const.h"

int postWeight(HX711 scale)
{
    HTTPClient http;
    int weight = (int) scale.get_units();
    String content = String(weight);

    http.begin(HOST_ENDPOINT);
    http.addHeader("Content-Type", "text/plain");

    int httpCode = http.POST(content);
    http.end();

    return httpCode;
}

void setLedStatus(enum LED_STATUS status)
{
    switch(status) {
      case LED_STATUS_RED:
        digitalWrite(PIN_LED_GREEN, HIGH);
        digitalWrite(PIN_LED_RED, LOW);
        break;
      case LED_STATUS_GREEN:
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_RED, HIGH);
        break;
      case LED_STATUS_YELLOW:
        digitalWrite(PIN_LED_GREEN, LOW);
        digitalWrite(PIN_LED_RED, LOW);
        break;        
      default:
        digitalWrite(PIN_LED_GREEN, HIGH);
        digitalWrite(PIN_LED_RED, HIGH);        
  }
}

void saveConfiguration(String ssid, String password, double offset)
{   
    File configFile = SPIFFS.open("/config.json", "w");

    StaticJsonDocument<1024> doc;

    doc["ssid"] = ssid;
    doc["password"] = password;
    doc["offset"] = offset;

    serializeJson(doc, configFile);

    configFile.close();
    return;
}

void resetConfiguration()
{        
    saveConfiguration("", "", 0.0);
    return;
}

void saveScaleOffset(double offset)
{
  saveConfiguration(getSavedSSID(), getSavedPassword(), offset);
}

double getSavedOffset()
{
    File configFile = SPIFFS.open("/config.json", "r");

    if (!configFile) {
        resetConfiguration();
        configFile = SPIFFS.open("/config.json", "r");
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, configFile);

    configFile.close();
  
    return doc["offset"];
}

String getSavedSSID()
{
    File configFile = SPIFFS.open("/config.json", "r");

    if (!configFile) {
        resetConfiguration();
        configFile = SPIFFS.open("/config.json", "r");
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, configFile);

    configFile.close();
  
    return doc["ssid"];
}

String getSavedPassword()
{
    File configFile = SPIFFS.open("/config.json", "r");

    if (!configFile) {
        resetConfiguration();
        configFile = SPIFFS.open("/config.json", "r");
    }

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, configFile);

    configFile.close();
  
    return doc["password"];
}

bool isAppletConfigured()
{
    return getSavedSSID() != "" && getSavedSSID() != "null";
}

void restartApplet()
{    
    optimistic_yield(500);
    ESP.restart();  
}
