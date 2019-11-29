#include <Arduino.h>

#define ARDUINOJSON_ENABLE_STD_STRING 1

#include <iostream>
#include "config.hpp"

#include <Streaming.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include <i2cSensorLib.h>

ADC_MODE(ADC_VCC)

Memory memory = Memory();
VCC vcc = VCC();
BH1750 bh1750 = BH1750();
SHT3X sht3x = SHT3X(0x45);
BMP180 bmp180 = BMP180();
BME280 bme280 = BME280();

std::string value;
unsigned long timerMeasureIntervallStart = 0;
static const int addressSwitchState = 0;
unsigned long timerSwitchValue = 0;
unsigned long timerSwitchStart = 0;

ESP8266WebServer webServer(serverPort);
PubSubClient pubSubClient;
Bounce sensorSwitch1 = Bounce();
Bounce sensorSwitch2 = Bounce();
WiFiClient wifiClient;
BearSSL::WiFiClientSecure wifiClientSecure;

std::string id;
std::string mqttTopicPublish;
std::string mqttTopicSubscribe;

void reset()
{
  delay(3000);
  ESP.reset();
  delay(5000);
}

bool verifyHostname()
{
  IPAddress result;

  return (WiFi.hostByName(mqtt_server.c_str(), result) == 1);
}

bool verifyFingerprint()
{
  wifiClientSecure.setFingerprint(mqtt_fingerprint.c_str());
  wifiClientSecure.connect(mqtt_server.c_str(), mqtt_port_secure);

  return (wifiClientSecure.connected());
}

int setSwitchStateFromEEPROM()
{
  EEPROM.begin(512);
  digitalWrite(SWITCH_PIN, EEPROM.read(addressSwitchState));
  EEPROM.end();

  return digitalRead(SWITCH_PIN);
}

void writeSwitchStateToEEPROM()
{
  EEPROM.begin(512);
  EEPROM.write(addressSwitchState, digitalRead(SWITCH_PIN));
  EEPROM.end();
}

std::string getValue()
{
  DynamicJsonDocument doc(1024);
  JsonArray sensorSwitchJson = doc.createNestedArray("sensorSwitch");
  JsonArray illuminanceJson = doc.createNestedArray("illuminance");
  JsonArray temperatureJson = doc.createNestedArray("temperature");
  JsonArray humidityJson = doc.createNestedArray("humidity");
  JsonArray pressureJson = doc.createNestedArray("pressure");
  std::string jsonString;

  doc["version"] = VERSION;
  doc["millis"] = millis();
  doc["hostname"] = WiFi.hostname();
  if (memory.isAvailable)
  {
    memory.getValues();
    doc["memory"] = memory.get(Sensor::MEMORY_MEASUREMENT);
  }
  if (vcc.isAvailable)
  {
    vcc.getValues();
    doc["vcc"] = vcc.get(Sensor::VOLTAGE_MEASUREMENT);
  }
  doc["switch"] = setSwitchStateFromEEPROM();
  sensorSwitchJson.add(digitalRead(SENSOR_PIN_1));
  sensorSwitchJson.add(digitalRead(SENSOR_PIN_2));
  if (bh1750.isAvailable)
  {
    bh1750.getValues();
    illuminanceJson.add(bh1750.get(Sensor::ILLUMINANCE_MEASUREMENT));
  }
  if (sht3x.isAvailable)
  {
    sht3x.getValues();
    temperatureJson.add(sht3x.get(Sensor::TEMPERATURE_MEASUREMENT));
    humidityJson.add(sht3x.get(Sensor::HUMIDITY_MEASUREMENT));
  }
  if (bmp180.isAvailable)
  {
    bmp180.getValues();
    temperatureJson.add(bmp180.get(Sensor::TEMPERATURE_MEASUREMENT));
    pressureJson.add(bmp180.get(Sensor::PRESSURE_MEASUREMENT));
  }
  if (bme280.isAvailable)
  {
    bme280.getValues();
    temperatureJson.add(bme280.get(Sensor::TEMPERATURE_MEASUREMENT));
    pressureJson.add(bme280.get(Sensor::PRESSURE_MEASUREMENT));
    humidityJson.add(bme280.get(Sensor::HUMIDITY_MEASUREMENT));
  }

  serializeJson(doc, jsonString);

  return jsonString;
}

void showValue(std::string value)
{
  std::cout << "value:                 " << value << std::endl;
}

void setupOTA()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.setPasswordHash(otaPasswordHash.c_str());

    ArduinoOTA.onStart([]() {
      std::string type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else
        type = "filesystem";
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      std::cout << "Start updating " + type << std::endl;
    });

    ArduinoOTA.onEnd([]() {
      std::cout << "\nEnd" << std::endl;
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      std::cout << "Progress: " << (int)(progress / (total / 100)) << "%" << std::endl;
    });

    ArduinoOTA.onError([](ota_error_t error) {
      //Serial.printf("Error[%u]: ", error);
      std::cout << "Error[" << error << "]: " << error << std::endl;

      switch (error)
      {
      case OTA_AUTH_ERROR:
        std::cout << "Auth Failed" << std::endl;
        break;
      case OTA_BEGIN_ERROR:
        std::cout << "Begin Failed" << std::endl;
        break;
      case OTA_CONNECT_ERROR:
        std::cout << "Connect Failed" << std::endl;
        break;
      case OTA_RECEIVE_ERROR:
        std::cout << "Receive Failed" << std::endl;
        break;
      case OTA_END_ERROR:
        std::cout << "End Failed" << std::endl;
        break;
      default:
        std::cout << "not identified" << std::endl;
      }
    });

    ArduinoOTA.begin();
  }
}

void setupWebServer()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    webServer.onNotFound([]() {
      webServer.send(200, "application/json", value.c_str());
    });

    webServer.begin();
  }
}

void setupID()
{
  byte mac[6];
  char buffer[13];

  WiFi.macAddress(mac);
  sprintf(buffer, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  id = std::string(buffer);
}

void connectMqtt()
{
  std::cout << "mqtt:                  ";
  std::cout << "connecting ... ";

  if (WiFi.status() == WL_CONNECTED)
  {
    if (pubSubClient.state() != 0)
    {
      if ((mqtt_username.length() == 0) || (mqtt_password.length() == 0))
      {
        std::cout << "(without Authentication) ";
        pubSubClient.connect(id.c_str());
      }
      else
      {
        std::cout << "(with Authentication) ";
        pubSubClient.connect(id.c_str(), mqtt_username.c_str(), mqtt_password.c_str());
      }
      if (pubSubClient.state() == 0)
      {
        std::cout << "connected" << std::endl;
        std::cout << "mqtt:                  " << mqttTopicSubscribe.c_str() << " ";
        if (pubSubClient.subscribe(mqttTopicSubscribe.c_str()))
          std::cout << "subscribed" << std::endl;
        else
          std::cout << "not subscribed" << std::endl;
      }
      else
      {
        std::cout << "not connected (rc=" << pubSubClient.state() << ")" << std::endl;
      }
    }
    else
    {
      std::cout << "still connected" << std::endl;
    }
  }
  else
  {
    std::cout << "not connected (no wifi)" << std::endl;
  }
}

void publishMqtt(std::string value)
{
  int length;

  Serial << "mqtt:                  ";
  Serial << "publishing ... ";

  if (WiFi.status() == WL_CONNECTED)
  {
    if (pubSubClient.state() == 0)
    {
      length = strlen(value.c_str());
      if (pubSubClient.beginPublish(mqttTopicPublish.c_str(), length, false))
      {
        pubSubClient.print(value.c_str());
        Serial << "published" << endl;
      }
      else
        Serial << "not published (transmit error)" << endl;
      pubSubClient.endPublish();
    }
    else
    {
      Serial << "not published (rc=" << pubSubClient.state() << ")" << endl;
      connectMqtt();
    }
  }
  else
    Serial << "not published (no wifi)" << endl;
}

void callback(char *topic, byte *payload, unsigned int length)
{
  char str[10];

  Serial << topic << ":   ";
  for (unsigned int i = 0; i < length; i++)
  {
    str[i] = (char)payload[i];
    Serial << (char)payload[i];
  }
  str[length] = '\0';
  Serial << endl;

  timerSwitchValue = String(str).toInt();

  if (timerSwitchValue < 2)
  {
    if ((timerSwitchValue == 0 ? false : true) != digitalRead(SWITCH_PIN))
    {
      digitalWrite(SWITCH_PIN, !digitalRead(SWITCH_PIN));
      writeSwitchStateToEEPROM();
      Serial << "switch:                ";
      timerSwitchValue == 0 ? Serial << "off" << endl : Serial << "on" << endl;
    }
  }
  else
  {
    digitalWrite(SWITCH_PIN, 1);
    writeSwitchStateToEEPROM();
    timerSwitchStart = millis();
    Serial << "switch:                on" << endl;
  }

  value = getValue();
  showValue(value);
  publishMqtt(value);
}

void setupMqttTopic()
{
  mqttTopicPublish = id + mqtt_value_prefix;
  mqttTopicSubscribe = id + mqtt_switch_prefix;
  std::cout << mqttTopicPublish << " " << mqttTopicSubscribe << std::endl;
}

void setupMqtt()
{
  if (verifyHostname())
  {
    if (verifyFingerprint())
    {
      pubSubClient.setClient(wifiClientSecure);
      pubSubClient.setServer(mqtt_server.c_str(), mqtt_port_secure);
    }
    else
    {
      pubSubClient.setClient(wifiClient);
      pubSubClient.setServer(mqtt_server.c_str(), mqtt_port);
    }

    pubSubClient.setCallback(callback);
  }
  else
  {
    std::cout << "failed to verify hostname" << std::endl;
    reset();
  }
}

void updateSensorSwitches()
{
  sensorSwitch1.update();
  sensorSwitch1.read();
  sensorSwitch2.update();
  sensorSwitch2.read();
}

void setupHardware()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(SENSOR_PIN_1, INPUT_PULLUP);
  pinMode(SENSOR_PIN_2, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, LOW);
  Wire.begin(SDA_PIN, SCL_PIN);
}

void displayWiFiStatus()
{
  digitalWrite(LED_BUILTIN, (WiFi.status() == WL_CONNECTED));
}

void printVersion()
{
  std::cout << std::endl
         << std::endl
         << "VERSION: " << VERSION << std::endl
         << std::endl;
}

void setupSensors()
{
  memory.begin();
  memory.isAvailable ? std::cout << "memory " : std::cout << ". ";

  vcc.begin();
  vcc.isAvailable ? std::cout << "vcc " : std::cout << ". ";

  bh1750.begin();
  bh1750.isAvailable ? std::cout << "bh1750 " : std::cout << ". ";

  sht3x.begin();
  sht3x.isAvailable ? std::cout << "sht3x " : std::cout << ". ";

  bmp180.begin();
  bmp180.isAvailable ? std::cout << "bmp180 " : std::cout << ". ";

  bme280.begin();
  bme280.isAvailable ? std::cout << "bme280  " << std::endl : std::cout << "." << std::endl;

  sensorSwitch1.attach(SENSOR_PIN_1);
  sensorSwitch1.interval(5);
  sensorSwitch2.attach(SENSOR_PIN_2);
  sensorSwitch2.interval(5);
}

void setupWiFi()
{
  WiFiManager wifiManager;

  WiFi.hostname(hostname.c_str());
  // wifiManager.resetSettings();
  // wifiManager.setDebugOutput(false);
  wifiManager.setConfigPortalTimeout(180);

  if (!wifiManager.autoConnect(hostname.c_str()))
  {
    std::cout  << "failed to connect and hit timeout" << std::endl;
    reset();
  }
}

void setup()
{
  Serial.begin(115200);

  printVersion();

  setupHardware();
  setSwitchStateFromEEPROM();
  setupWiFi();
  displayWiFiStatus();

  setupSensors();

  setupOTA();
  setupWebServer();

  setupID();
  setupMqttTopic();
  setupMqtt();
  connectMqtt();
}

void loop()
{
  displayWiFiStatus();
  pubSubClient.loop();
  updateSensorSwitches();

  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.handle();
    webServer.handleClient();
  }

  if (millis() - timerMeasureIntervallStart > timerMeasureIntervall * 1000UL)
  {
    timerMeasureIntervallStart = millis();
    value = getValue();
    showValue(value);
    publishMqtt(value);
  }

  if (timerSwitchValue > 1 and digitalRead(SWITCH_PIN) == 1 and millis() - timerSwitchStart > timerSwitchValue)
  {
    digitalWrite(SWITCH_PIN, 0);
    writeSwitchStateToEEPROM();
    Serial << "switch:                off" << endl;
    value = getValue();
    showValue(value);
    publishMqtt(value);
  }

  yield();
}
