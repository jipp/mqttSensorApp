#ifndef config_h
#define config_h

#ifndef VERSION
#define VERSION "mqttSensorApp"
#endif

//#define DEEPSLEEP 30

#define WIFI_MODE WIFI_AP
//#define WIFI_MODE WIFI_STA

const char* ssid_AP = "<wifi-ap>";

const char* ssid_1 = "<wifi-ssid>";
const char* password_1 = "<wifi-key>";
const char* ssid_2 = "<wifi-ssid>";
const char* password_2 = "<wifi-key>";
const char* ssid_3 = "<wifi-ssid>";
const char* password_3 = "<wifi-key>";
const int retryLimit = 50;
const int retryTimer = 30;

const unsigned long timerMeasureIntervall = 10;

const char* mqtt_server = "<mqtt-broker>";
const char* mqtt_port = "<port>";
const char* mqtt_username = "<mqtt-user>";
const char* mqtt_password = "<mqtt-password>";
const char* mqtt_topic_prefix = "<mqtt-topic-prefix>";

#endif
