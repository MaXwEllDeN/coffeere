#ifndef _H_FUNCTIONS
#define _H_FUNCTIONS

#include "Arduino.h"
#include "HX711.h"
#include "const.h"

void saveConfiguration(String ssid, String password, double offset);
void resetConfiguration();
double getSavedOffset();
void saveScaleOffset(double offset);
String getSavedSSID();
String getSavedPassword();
bool isAppletConfigured();
void restartApplet();
int postWeight(HX711 scale);
void setLedStatus(enum LED_STATUS status);

#endif
