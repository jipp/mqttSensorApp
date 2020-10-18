#include <Arduino.h>

#define ARDUINOJSON_ENABLE_STD_STRING 1

#include <iomanip>
#include <iostream>
#include <sstream>

#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncMqttClient.h>
#include <Bounce2.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiManager.h>
#include <Wire.h>

#include "config.hpp"
#include <BH1750.hpp>
#include <BME280.hpp>
#include <BMP180.hpp>
#include <Dummy.hpp>
#include <Memory.hpp>
#include <SHT3X.hpp>
#include <VCC.hpp>

#ifndef SPEED
#define SPEED 115200
#endif

ADC_MODE(ADC_VCC)

ESP8266WebServer webServer(webServerPort);

WiFiEventHandler connectedEventHandler;
WiFiEventHandler disconnectedEventHandler;
WiFiEventHandler gotIpEventHandler;
WiFiEventHandler DHCPTimeoutHandler;
Ticker wifiReconnect;
std::string psk;
std::string ssid;

AsyncMqttClient mqttClient;
Ticker mqttReconnect;
Ticker mqttPublish;
std::string id;
std::string mqttPublishTopic;
std::string mqttSubscribeTopic;

std::string value;

Memory memory = Memory();
VCC vcc = VCC();
BH1750 bh1750 = BH1750();
SHT3X sht3x = SHT3X(0x45);
BMP180 bmp180 = BMP180();
BME280 bme280 = BME280();
Bounce sensorSwitch1 = Bounce();
Bounce sensorSwitch2 = Bounce();
static const int debounceInterval = 5;
Ticker switchTimer;

static const int eepromAddress = 512;
static const int addressSwitchState = 0;

constexpr int HTTP_OK = 200;
constexpr int HTTP_NOT_FOUND = 404;

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
  sensorSwitchJson.add(sensorSwitch1.read());
  sensorSwitchJson.add(sensorSwitch2.read());
  doc["switch"] = (digitalRead(SWITCH_PIN) == 1);
  if (memory.isAvailable)
  {
    memory.getValues();
    doc["memory"] = memory.get(Measurement::MEMORY);
  }
  if (vcc.isAvailable)
  {
    vcc.getValues();
    doc["vcc"] = vcc.get(Measurement::VOLTAGE);
  }
  if (bh1750.isAvailable)
  {
    bh1750.getValues();
    illuminanceJson.add(bh1750.get(Measurement::ILLUMINANCE));
  }
  if (sht3x.isAvailable)
  {
    sht3x.getValues();
    temperatureJson.add(sht3x.get(Measurement::TEMPERATURE));
    humidityJson.add(sht3x.get(Measurement::HUMIDITY));
  }
  if (bmp180.isAvailable)
  {
    bmp180.getValues();
    temperatureJson.add(bmp180.get(Measurement::TEMPERATURE));
    pressureJson.add(bmp180.get(Measurement::PRESSURE));
  }
  if (bme280.isAvailable)
  {
    bme280.getValues();
    temperatureJson.add(bme280.get(Measurement::TEMPERATURE));
    pressureJson.add(bme280.get(Measurement::PRESSURE));
    humidityJson.add(bme280.get(Measurement::HUMIDITY));
  }

  serializeJson(doc, jsonString);

  return jsonString;
}

void setSwitchFromEEPROM()
{
  EEPROM.begin(eepromAddress);
  digitalWrite(SWITCH_PIN, EEPROM.read(addressSwitchState));
  EEPROM.end();
}

void setEEPROMfromSwitch()
{
  EEPROM.begin(eepromAddress);
  EEPROM.write(addressSwitchState, digitalRead(SWITCH_PIN));
  EEPROM.end();
}

void wifiConnect()
{
  std::cout << "Connecting to Wi-Fi..." << std::endl;
  WiFi.begin(ssid.c_str(), psk.c_str());
}

void connectToMqtt()
{
  std::cout << "Connecting to MQTT..." << std::endl;
  mqttClient.connect();
}

void onConnected(const WiFiEventStationModeConnected &event)
{
  std::cout << "Connected to Wi-Fi." << std::endl;
}

void onDisconnected(const WiFiEventStationModeDisconnected &event)
{
  std::cout << "Disconnected from Wi-Fi." << std::endl;
  digitalWrite(LED_BUILTIN, false);
  mqttPublish.detach();
  mqttReconnect.detach();
  webServer.stop();
  wifiReconnect.once(2, wifiConnect);
}

void onGotIp(const WiFiEventStationModeGotIP &event)
{
  std::cout << "Got IP: " << std::string(WiFi.localIP().toString().c_str()) << std::endl;
  digitalWrite(LED_BUILTIN, true);
  connectToMqtt();
  webServer.begin();
  ArduinoOTA.begin();
}

void onDHCPTimeout()
{
  std::cout << "DHCP Timeout." << std::endl;
}

void getWiFi()
{
  const uint64 timeout = 180;
  WiFiManager wifiManager;

  WiFi.hostname(hostname.c_str());
  // wifiManager.resetSettings();
  wifiManager.setDebugOutput(false);
  wifiManager.setConfigPortalTimeout(timeout);

  if (!wifiManager.autoConnect(hostname.c_str()))
  {
    std::cout << "failed to connect and hit timeout" << std::endl;
  }

  psk = std::string(WiFi.psk().c_str());
  ssid = std::string(WiFi.SSID().c_str());
}

void mqttPublishMessage()
{
  value = getValue();
  uint16_t packetIdPub = mqttClient.publish(mqttPublishTopic.c_str(), mqttPublishQoS, true, value.c_str());
  std::cout << "Publishing at QoS " << unsigned(mqttPublishQoS) << ", packetId: " << packetIdPub << std::endl;
}

void onMqttConnect(bool sessionPresent)
{
  std::cout << "Connected to MQTT." << std::endl;
  std::cout << "Session present: " << sessionPresent << std::endl;
  uint16_t packetIdSub = mqttClient.subscribe(mqttSubscribeTopic.c_str(), mqttSubscribeQoS);
  std::cout << "Subscribing at QoS " << unsigned(mqttSubscribeQoS) << ", packetId: " << packetIdSub << std::endl;
  mqttPublish.attach(measureIntervall, mqttPublishMessage);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  std::cout << "Disconnected from MQTT." << std::endl;

  if (reason == AsyncMqttClientDisconnectReason::TLS_BAD_FINGERPRINT)
  {
    std::cout << "Bad server fingerprint." << std::endl;
  }

  if (WiFi.isConnected())
  {
    mqttReconnect.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  std::cout << "Subscribe acknowledged." << std::endl;
  std::cout << "  packetId: " << packetId << std::endl;
  std::cout << "  qos: " << unsigned(qos) << std::endl;
}

void onMqttUnsubscribe(uint16_t packetId)
{
  std::cout << "Unsubscribe acknowledged." << std::endl;
  std::cout << "  packetId: " << packetId << std::endl;
}

void switchOn()
{
  digitalWrite(SWITCH_PIN, 1);
  setEEPROMfromSwitch();
  mqttPublishMessage();
}

void switchOff()
{
  digitalWrite(SWITCH_PIN, 0);
  setEEPROMfromSwitch();
  mqttPublishMessage();
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  int value = 0;

  std::cout << "Publish received." << std::endl;
  std::cout << "  topic: " << topic << std::endl;
  std::cout << "  qos: " << unsigned(properties.qos) << std::endl;
  std::cout << "  dup: " << properties.dup << std::endl;
  std::cout << "  retain: " << properties.retain << std::endl;
  std::cout << "  len: " << len << std::endl;
  std::cout << "  index: " << index << std::endl;
  std::cout << "  total: " << total << std::endl;
  payload[len] = '\0';
  std::cout << "  payload: " << payload << std::endl;

  if (strcmp(payload, "true") == 0)
  {
    value = 1;
  }
  else if (strcmp(payload, "false") == 0)
  {
    value = 0;
  }
  else
  {
    std::istringstream(payload) >> value;
  }

  std::cout << "  value: " << value << std::endl;

  switch (value)
  {
  case 0:
    switchOff();
    switchTimer.detach();
    break;
  case 1:
    switchOn();
    switchTimer.detach();
    break;
  default:
    switchOn();
    switchTimer.once_ms(value, switchOff);
    break;
  }
}

void onMqttPublish(uint16_t packetId)
{
  std::cout << "Publish acknowledged." << std::endl;
  std::cout << "  packetId: " << packetId << std::endl;
}

void setup()
{
  // general setup
  Serial.begin(SPEED);
  setSwitchFromEEPROM();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_PIN, OUTPUT);
  pinMode(SENSOR_PIN_1, INPUT_PULLUP);
  pinMode(SENSOR_PIN_2, INPUT_PULLUP);
  Wire.begin(SDA_PIN, SCL_PIN);

  // setup sensors
  sensorSwitch1.attach(SENSOR_PIN_1);
  sensorSwitch1.interval(debounceInterval);
  sensorSwitch2.attach(SENSOR_PIN_2);
  sensorSwitch2.interval(debounceInterval);
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

  // setup WiFi
  connectedEventHandler = WiFi.onStationModeConnected(onConnected);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(onDisconnected);
  gotIpEventHandler = WiFi.onStationModeGotIP(onGotIp);
  DHCPTimeoutHandler = WiFi.onStationModeDHCPTimeout(onDHCPTimeout);

  // setup mqtt
  byte mac[6];
  std::ostringstream stringStream;
  WiFi.macAddress(mac);
  stringStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[0])
               << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[1])
               << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[2])
               << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[3])
               << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[4])
               << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(mac[5]);
  id = stringStream.str();
  mqttPublishTopic = id + mqttValuePrefix;
  mqttSubscribeTopic = id + mqttSwitchPrefix;

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  if (!mqttUsername.empty() or !mqttPassword.empty())
  {
    mqttClient.setCredentials(mqttUsername.c_str(), mqttPassword.c_str());
  }
  if (sizeof(mqttFingerprint) == 20)
  {
    mqttClient.setServer(mqttServer.c_str(), mqttPortSecure);
    mqttClient.setSecure(true);
    mqttClient.addServerFingerprint(mqttFingerprint);
  }
  else
  {
    mqttClient.setServer(mqttServer.c_str(), mqttPort);
  }
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  // setup webserver
  webServer.onNotFound([]() {
    webServer.send(HTTP_NOT_FOUND, "text/plain", "Not found");
  });
  webServer.on("/", []() {
    webServer.send(HTTP_OK, "application/json", getValue().c_str());
  });

  // setup OTA
  ArduinoOTA.setPasswordHash(otaPasswordHash.c_str());
  ArduinoOTA.onStart([]() {
    std::string type;
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    {
      type = "filesystem";
    } // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    std::cout << "Start updating " + type << std::endl;
  });
  ArduinoOTA.onEnd([]() {
    std::cout << "\nEnd" << std::endl;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    std::cout << "Progress: " << static_cast<int>(progress / (total / 100)) << "%" << std::endl;
  });
  ArduinoOTA.onError([](ota_error_t error) {
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

  // start
  getWiFi();
}

void loop()
{
  sensorSwitch1.update();
  sensorSwitch2.update();
  if (WiFi.status() == WL_CONNECTED)
  {
    webServer.handleClient();
    ArduinoOTA.handle();
  }
}