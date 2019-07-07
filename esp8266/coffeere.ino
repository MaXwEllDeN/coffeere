#include "FS.h"
#include "const.h"
#include "webserver.h"
#include "functions.h"
#include "HX711.h"

MODULE_MODE operation_mode;

Webserver server;
HX711 scale;

int counterToPost = 0;
int counterWifiTimeout = 0;
int counterToReset = 0;
void startConfigurationMode()
{
  WiFi.mode(WIFI_AP_STA);

  IPAddress ap_ip(18, 13, 0, APPLET_IP_IDENTIFIER);
  WiFi.softAPConfig(ap_ip, IPAddress(18, 13, 0, 0), IPAddress(255, 255, 255, 0));
  WiFi.softAP(AP_SSID);

  server.begin();
}

void setup() {
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  pinMode(PIN_BUTTON_TARE, INPUT);
  pinMode(PIN_BUTTON_RESET, INPUT);

  setLedStatus(LED_STATUS_RED);

  Serial.begin(115200);
  delay(10);

  SPIFFS.begin();

  if (!isAppletConfigured()) {
    operation_mode = CONFIGURATION_MODE;
    startConfigurationMode();
  }
  else {
    scale.begin(PIN_HX711_DOUT, PIN_HX711_CLK);
    WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
    delay(1000);

    scale.set_scale(SCALE_CALIBRATION_FACTOR);
    scale.set_offset(getSavedOffset());

    Serial.print("Saved ScaleOffset: ");
    Serial.println(getSavedOffset());

    WiFi.mode(WIFI_STA);
    WiFi.begin(getSavedSSID().c_str(), getSavedPassword().c_str());

    #ifdef __DEBUG__
    Serial.println("");
    Serial.print("Connecting");
    #endif // __DEBUG__

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      if (counterWifiTimeout >= TIMEOUT_WIFI_CONNECT) {
        break;
      }

      counterWifiTimeout += 500;
      delay(500);

      #ifdef __DEBUG__      
      Serial.print(".");
      #endif // __DEBUG__
    }

    if (WiFi.status() != WL_CONNECTED) {
      operation_mode = CONFIGURATION_MODE;
      startConfigurationMode();
    }
    else {
      operation_mode = WORKING_MODE;
      setLedStatus(LED_STATUS_GREEN);

      #ifdef __DEBUG__
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      #endif // __DEBUG__      
    }
  }
}

void loop() {
  if (operation_mode == CONFIGURATION_MODE) {
    server.getServerObject()->handleClient();
  }
  else if (operation_mode == WORKING_MODE) {
    if (!digitalRead(PIN_BUTTON_TARE)) {
      setLedStatus(LED_STATUS_RED);

      double sum = scale.read_average();
      scale.set_offset(sum);
      saveScaleOffset(sum);
      delay(500);

      counterToPost += 500;
    }

    while(!digitalRead(PIN_BUTTON_RESET)) {
      setLedStatus(LED_STATUS_YELLOW);

      if (counterToReset >= TIME_PRESSING_TO_RESET) {
        setLedStatus(LED_STATUS_RED);
        resetConfiguration();       
        restartApplet();
        break;
      }

      counterToReset += 100;
      counterToPost += 100;

      delay(100);
    }

    counterToReset = 0;    

    if (counterToPost >= 2000) {
      int httpCode = postWeight(scale);
      counterToPost = 0;
    }

    setLedStatus(LED_STATUS_GREEN);
    counterToPost += 100;
    delay(100);
  }
}
