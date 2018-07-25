#ifndef config_h
#define config_h

#ifndef TIMER
#define TIMER 60
#endif

#ifndef VERSION
#define VERSION "mqttSensorApp"
#endif

const char* ssid_1 = "<wifi-ssid>";
const char* password_1 = "<wifi-key>";
const char* ssid_2 = "<wifi-ssid>";
const char* password_2 = "<wifi-key>";
const char* ssid_3 = "<wifi-ssid>";
const char* password_3 = "<wifi-key>";

const char* mqtt_server = "<mqtt-broker>";
const char* mqtt_port = "<port>";
const char* mqtt_username = "<mqtt-user>";
const char* mqtt_password = "<mqtt-password>";
const char* mqtt_topic_prefix = "<mqtt-topic-prefix>";

const unsigned long timerMeasureIntervall = TIMER;
const unsigned long timerLastReconnect = 60;

const char* data = "/data.txt";
const char* dataTemp = "/dataTemp.txt";
const int bufferLength = 20;

#endif
