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
static const int webServerPort = 80;

// measurement intervall
static const uint32_t measureIntervall = 10;

// mqtt settings
static const std::string mqttServer = "<mqtt-broker>";
static const uint16_t mqttPort PROGMEM = 1883;
static const uint16_t mqttPortSecure PROGMEM = 8883;
static const std::string mqttFingerprint = "<fingerprint>";
static const std::string mqttUsername = "<mqtt-user>";
static const std::string mqttPassword =  "<mqtt-password>";
static const std::string mqttValuePrefix = "<mqtt-topic-value-prefix>";
static const std::string mqttSwitchPrefix = "<mqtt-topic-switch-prefix>";
static const uint8_t mqttPublishQoS = 0;
static const uint8_t mqttSubscribeQoS = 0;

#endif
