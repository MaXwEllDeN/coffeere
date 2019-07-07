#ifndef _H_CONSTANT
#define _H_CONSTANT

#define APPLET_IP_IDENTIFIER          1
#define TIMEOUT_WIFI_CONNECT          60000
#define TIME_PRESSING_TO_RESET        5000
#define SCALE_CALIBRATION_FACTOR      300

#define PIN_HX711_DOUT      14
#define PIN_HX711_CLK       16

#define PIN_LED_RED         13
#define PIN_LED_GREEN       12
#define PIN_BUTTON_RESET    4
#define PIN_BUTTON_TARE     2

enum MODULE_MODE {
  CONFIGURATION_MODE,
  WORKING_MODE
};

enum LED_STATUS {
  LED_STATUS_OFF,
  LED_STATUS_RED,
  LED_STATUS_GREEN,
  LED_STATUS_YELLOW
};

static const char *HOST_ENDPOINT = "http://maxwellden.pythonanywhere.com/weight";
static const char *AP_SSID = "Coffeere";

#endif // _H_CONSTANT
