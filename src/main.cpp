#include <Arduino.h>

#include "config.h"

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

ADC_MODE(ADC_VCC);

Memory memory = Memory();
VCC vcc = VCC();
BH1750 bh1750 = BH1750();
SHT3X sht3x = SHT3X(0x45);
BMP180 bmp180 = BMP180();
BME280 bme280 = BME280();

String value;
unsigned long timerMeasureIntervallStart = 0;
static const int addressSwitchState = 0;
unsigned long timerSwitchValue = 0;
unsigned long timerSwitchStart = 0;

ESP8266WebServer server(serverPort);
PubSubClient pubSubClient;
Bounce sensorSwitch1 = Bounce();
Bounce sensorSwitch2 = Bounce();
WiFiClient wifiClient;
BearSSL::WiFiClientSecure wifiClientSecure;

char id[13];
String mqttTopicPublish;
String mqttTopicSubscribe;

void reset()
{
  delay(3000);
  ESP.reset();
  delay(5000);
}

bool verifyHostname()
{
  IPAddress result;

  Serial << "server address check:  ";

  if (String(mqtt_server).length() != 0)
  {
    Serial << "(" << mqtt_server << ") ";
    if (WiFi.hostByName(mqtt_server, result) == 1)
    {
      Serial << "OK" << endl;
      return true;
    }
    else
    {
      Serial << "NOK" << endl;
      return false;
    }
  }

  Serial << "not defined" << endl;

  return false;
}

bool verifyFingerprint()
{
  wifiClientSecure.setFingerprint(mqtt_fingerprint);

  Serial << "TLS connect:           ";
  wifiClientSecure.connect(mqtt_server, String(mqtt_port_secure).toInt());

  if (wifiClientSecure.connected())
  {
    Serial << "OK" << endl;
    return true;
  }
  else
  {
    Serial << "NOK" << endl;
    return false;
  }
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

String getValue()
{
  DynamicJsonDocument doc(1024);
  JsonArray sensorSwitchJson = doc.createNestedArray("sensorSwitch");
  JsonArray illuminanceJson = doc.createNestedArray("illuminance");
  JsonArray temperatureJson = doc.createNestedArray("temperature");
  JsonArray humidityJson = doc.createNestedArray("humidity");
  JsonArray pressureJson = doc.createNestedArray("pressure");
  String jsonString;

  doc["version"] = VERSION;
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

void showValue(String value)
{
  Serial << "value:                 " << value << endl;
}

void setupOTA()
{
  Serial << "OTA:                   ";
  if (WiFi.status() == WL_CONNECTED)
  {
    ArduinoOTA.setPasswordHash(otaPasswordHash);

    ArduinoOTA.onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else
        type = "filesystem";
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR)
        Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR)
        Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR)
        Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial << "started" << endl;
  }
  else
    Serial << "not started" << endl;
}

void setupWebServer()
{
  Serial << "web server:            ";
  Serial << "(port: " << serverPort << ") ";
  if (WiFi.status() == WL_CONNECTED)
  {
    server.onNotFound([]() {
      server.send(200, "application/json", value);
    });

    server.begin();
    Serial << "OK" << endl;
  }
  else
    Serial << "NOK" << endl;
}

void setupID()
{
  byte mac[6];

  Serial << "id:                    ";
  WiFi.macAddress(mac);
  sprintf(id, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << id << endl;
}

void connectMqtt()
{
  Serial << "mqtt:                  ";
  Serial << "connecting ... ";

  if (WiFi.status() == WL_CONNECTED)
  {
    if (pubSubClient.state() != 0)
    {
      if ((String(mqtt_username).length() == 0) || (String(mqtt_password).length() == 0))
      {
        Serial << "(without Authentication) ";
        pubSubClient.connect(id);
      }
      else
      {
        Serial << "(with Authentication) ";
        pubSubClient.connect(id, String(mqtt_username).c_str(), String(mqtt_password).c_str());
      }
      if (pubSubClient.state() == 0)
      {
        Serial << "connected" << endl;
        Serial << "mqtt:                  " << mqttTopicSubscribe << " ";
        if (pubSubClient.subscribe(String(mqttTopicSubscribe).c_str()))
          Serial << "subscribed" << endl;
        else
          Serial << "not subscribed" << endl;
      }
      else
      {
        Serial << "not connected (rc=" << pubSubClient.state() << ")" << endl;
      }
    }
    else
    {
      Serial << "still connected" << endl;
    }
  }
  else
  {
    Serial << "not connected (no wifi)" << endl;
  }
}

void publishMqtt(String value)
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
        pubSubClient.print(value);
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
  Serial << "mqtt publish topic:    ";
  mqttTopicPublish = id + String(mqtt_value_prefix);
  Serial << mqttTopicPublish << endl;
  Serial << "mqtt subcribe topic:   ";
  mqttTopicSubscribe = id + String(mqtt_switch_prefix);
  Serial << mqttTopicSubscribe << endl;
}

void setupMqtt()
{
  if (verifyHostname() == 1)
  {
    if (mqtt_use_secure)
      Serial << "mqtt secure port:      " << mqtt_port_secure << endl;
    else
      Serial << "mqtt port:             " << mqtt_port << endl;

    if (mqtt_use_secure)
    {
      if (!verifyFingerprint())
      {
        Serial << "failed to verify fingerprint" << endl;
        reset();
      }
      pubSubClient.setClient(wifiClientSecure);
      pubSubClient.setServer(mqtt_server, String(mqtt_port_secure).toInt());
    }
    else
    {
      pubSubClient.setClient(wifiClient);
      pubSubClient.setServer(mqtt_server, String(mqtt_port).toInt());
    }

    pubSubClient.setCallback(callback);
  }
  else
  {
    Serial << "failed to verify hostname" << endl;
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
  if (WiFi.status() == WL_CONNECTED)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
}

void printVersion()
{
  Serial << endl
         << endl
         << "VERSION:               " << VERSION << endl
         << endl;
}

void setupSensors()
{
  memory.begin();
  Serial << "memory:                ";
  memory.isAvailable ? Serial << "OK" << endl : Serial << "NOK" << endl;

  vcc.begin();
  Serial << "vcc:                   ";
  vcc.isAvailable ? Serial << "OK" << endl : Serial << "NOK" << endl;

  bh1750.begin();
  Serial << "bh1750:                ";
  bh1750.isAvailable ? Serial << "OK" << endl : Serial << "NOK" << endl;

  sht3x.begin();
  Serial << "sht3x:                 ";
  sht3x.isAvailable ? Serial << "OK" << endl : Serial << "NOK" << endl;

  bmp180.begin();
  Serial << "bmp180:                ";
  bmp180.isAvailable ? Serial << "OK" << endl : Serial << "NOK" << endl;

  bme280.begin();
  Serial << "bme280:                ";
  bme280.isAvailable ? Serial << "OK" << endl : Serial << "NOK" << endl;

  sensorSwitch1.attach(SENSOR_PIN_1);
  sensorSwitch1.interval(5);
  sensorSwitch2.attach(SENSOR_PIN_2);
  sensorSwitch2.interval(5);
}

bool connectWiFi()
{
  int count = 0;

  Serial << "WiFi:                  ";
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial << ".";
    if (count++ > 20)
    {
      Serial << "NOK" << endl;
      return false;
    }
  }

  Serial << "OK" << endl;

  return true;
}

void setupWiFi()
{
  WiFiManager wifiManager;

  WiFi.hostname(hostname);
  // wifiManager.resetSettings();
  // wifiManager.setDebugOutput(false);
  wifiManager.setConfigPortalTimeout(180);

  if (!wifiManager.autoConnect(hostname))
  {
    Serial << "failed to connect and hit timeout" << endl;
    reset();
  }
}

void printWiFi()
{
  Serial << "SSID:                  " << WiFi.SSID() << endl;
  Serial << "RSSI:                  " << WiFi.RSSI() << endl;
  Serial << "Hostname:              " << WiFi.hostname() << endl;
  Serial << "IP:                    " << WiFi.localIP() << endl;
}

void setup()
{
  Serial.begin(115200);

  printVersion();

  setupHardware();
  setSwitchStateFromEEPROM();
  setupWiFi();
  printWiFi();
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
    server.handleClient();
  }

  if (millis() - timerMeasureIntervallStart > timerMeasureIntervall * 1000)
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
