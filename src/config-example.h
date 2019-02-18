#ifndef config_h
#define config_h

#ifndef VERSION
#define VERSION "mqttSensorApp"
#endif

// i2c pins
#define SCL_PIN D1 // D1 default for wemos
#define SDA_PIN D2 // D2 default for wemos

// switch Pin
#define SWITCH_PIN D5 // D1 default for relay shield v1.0.0

// sensor Pin
#define SENSOR_PIN_1 D6
#define SENSOR_PIN_2 D7

// ota settings
const char *otaPasswordHash PROGMEM = "<md5 hash for password>";

// wifi settings
const char *hostname PROGMEM = "<hostname>";

// webserver port
const int serverPort = 80;

// measurement intervall
const unsigned long timerMeasureIntervall = 10;

// mqtt settings
const bool mqtt_use_secure = false;
const char *mqtt_server PROGMEM = "<mqtt-broker>";
const char *mqtt_port PROGMEM = "<port>";
const char *mqtt_port_secure PROGMEM = "<secure port>";
const char *mqtt_fingerprint PROGMEM = "<fingerprint>";
const char *mqtt_username PROGMEM = "<mqtt-user>";
const char *mqtt_password PROGMEM = "<mqtt-password>";
const char *mqtt_value_prefix PROGMEM = "<mqtt-topic-value-prefix>";
const char *mqtt_switch_prefix PROGMEM = "<mqtt-topic-switch-prefix>";

#endif
