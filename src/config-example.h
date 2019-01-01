#ifndef config_h
#define config_h

#ifndef VERSION
#define VERSION "mqttSensorApp"
#endif

// i2c pins
#define SCL_PIN D1 // default for wemos
#define SDA_PIN D2 // default for wemos

// switch Pin
#define SWITCH_PIN D3 // defaul for relay shield (D1)

// deepsleep setting
// #define DEEPSLEEP 30

// wifi settings
//#define WIFI_MODE WIFI_AP
#define WIFI_MODE WIFI_STA

const char* ssid_AP = "<wifi-ap>";
const char* ssid_1 = "<wifi-ssid>";
const char* password_1 = "<wifi-key>";
const char* ssid_2 = "<wifi-ssid>";
const char* password_2 = "<wifi-key>";
const char* ssid_3 = "<wifi-ssid>";
const char* password_3 = "<wifi-key>";
const int retryLimit = 50;
const int retryTimer = 30;

// measurement intervall
const unsigned long timerMeasureIntervall = 10;

// mqtt settings
const char* mqtt_server = "<mqtt-broker>";
const char* mqtt_port = "<port>";
const char* mqtt_username = "<mqtt-user>";
const char* mqtt_password = "<mqtt-password>";
const char* mqtt_value_prefix = "<mqtt-topic-value-prefix>";
const char* mqtt_switch_prefix = "<mqtt-topic-switch-prefix>";

#endif
