#ifndef CONFIG_HPP
#define CONFIG_HPP

#ifndef VERSION
#define VERSION "mqttSensorApp"
#endif

// i2c pins
static const uint8_t SCL_PIN = D1; // D1 default for wemos
static const uint8_t SDA_PIN = D2; // D2 default for wemos

// switch Pin
static const uint8_t SWITCH_PIN = D5; // D1 default for relay shield v1.0.0

// sensor Pin
static const uint8_t SENSOR_PIN_1 = D6;
static const uint8_t SENSOR_PIN_2 = D7;

// ota settings
static const std::string otaPasswordHash = "<md5 hash for password>";

// wifi settings
static const std::string hostname = "<hostname>";

// webserver port
static const int serverPort = 80;

// measurement intervall
static const uint32_t timerMeasureIntervall = 10;

// mqtt settings
static const std::string mqtt_server = "<mqtt-broker>";
static const uint16_t mqtt_port PROGMEM = 1883;
static const uint16_t mqtt_port_secure PROGMEM = 8883;
static const std::string mqtt_fingerprint = "<fingerprint>";
static const std::string mqtt_username = "<mqtt-user>";
static const std::string mqtt_password =  "<mqtt-password>";
static const std::string mqtt_value_prefix = "<mqtt-topic-value-prefix>";
static const std::string mqtt_switch_prefix = "<mqtt-topic-switch-prefix>";

#endif
