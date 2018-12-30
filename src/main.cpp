#include <Arduino.h>

#include "config.h"

#include <Streaming.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>

#include <i2cSensorLib.h>

ADC_MODE(ADC_VCC);

VCC vcc = VCC();
BH1750 bh1750 = BH1750();
SHT3X sht3x = SHT3X(0x45);
BMP180 bmp180 = BMP180();
BME280 bme280 = BME280();

String value;
unsigned long timerMeasureIntervallStart = 0;
const int address = 0;
unsigned long timerSwitchValue = 0;
unsigned long timerSwitchStart = 0;

ESP8266WebServer server(80);
ESP8266WiFiMulti wifiMulti;
PubSubClient pubSubClient;
WiFiClient wifiClient;

char id[13];
String mqttTopicPublish;
String mqttTopicSubscribe;

void setLED()
{
  if (WiFi.getMode() == WIFI_AP or wifiMulti.run() == WL_CONNECTED)
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
}

void readSwitchStateEEPROM()
{
  EEPROM.begin(512);
  digitalWrite(SWITCH_BUTTON, EEPROM.read(address));
}

void writeSwitchStateEEPROM()
{
  EEPROM.write(address, digitalRead(SWITCH_BUTTON));
  EEPROM.commit();
}

String getValue()
{
  DynamicJsonBuffer jsonBuffer;
  JsonObject &jsonObject = jsonBuffer.createObject();
  JsonArray &vccJson = jsonObject.createNestedArray("vcc");
  JsonArray &illuminanceJson = jsonObject.createNestedArray("illuminance");
  JsonArray &temperatureJson = jsonObject.createNestedArray("temperature");
  JsonArray &humidityJson = jsonObject.createNestedArray("humidity");
  JsonArray &pressureJson = jsonObject.createNestedArray("pressure");
  String jsonString;

  if (vcc.isAvailable)
  {
    vcc.getValues();
    vccJson.add(vcc.get(Sensor::VOLTAGE_MEASUREMENT));
  }
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

  jsonString = "";
  jsonObject.printTo(jsonString);

  return jsonString;
}

void showValue(String value)
{
  Serial << "value:                 " << value << endl;
}

void setupWiFi(WiFiMode_t mode)
{
  Serial << "WiFi Mode:             ";
  if (mode == WIFI_AP)
  {
    WiFi.mode(mode);
    WiFi.softAP(ssid_AP);
    Serial << WiFi.getMode() << endl;
    Serial << "IP:                             " << WiFi.softAPIP() << endl;
  }
  if (mode == WIFI_STA)
  {
    WiFi.mode(mode);
    wifiMulti.addAP(ssid_STA_1, password_STA_1);
    wifiMulti.addAP(ssid_STA_2, password_STA_2);
    wifiMulti.addAP(ssid_STA_3, password_STA_3);
    Serial << WiFi.getMode() << endl;
    Serial << "hostname:              " << WiFi.hostname() << endl;
  }
}

void connectWiFi()
{
  int retryCounter = 0;

  if (WiFi.getMode() == WIFI_STA)
  {
    Serial << "WiFi:                  connecting ";
    while (wifiMulti.run() != WL_CONNECTED)
    {
      Serial << ".";
      delay(100);
      retryCounter++;
      if (retryCounter > retryLimit)
      {
        Serial << " not connected" << endl;
        digitalWrite(LED_BUILTIN, LOW);
        Serial << "              restarting now and retrying in " << retryTimer << " sec" << endl;
        yield();
        ESP.deepSleep(retryTimer * 1000000);
        //        ESP.restart();
      }
    }
    Serial << " connected" << endl;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial << "IP:                    " << WiFi.localIP() << endl;
    Serial << "RSSI:                  " << WiFi.RSSI() << endl;
  }
}

void setupOTA()
{
  Serial << "OTA:                   ";
  if (WiFi.getMode() == WIFI_AP or WiFi.status() == WL_CONNECTED)
  {

    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });

    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      ;
      Serial.println();
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
  Serial << "Web Server:            ";
  if (WiFi.getMode() == WIFI_AP or WiFi.status() == WL_CONNECTED)
  {

    server.onNotFound([]() {
      server.send(200, "application/json", value);
    });

    server.begin();
    Serial << "started" << endl;
  }
  else
    Serial << "not started" << endl;
}

void setupID()
{
  byte mac[6];

  Serial << "id:                    ";
  WiFi.macAddress(mac);
  sprintf(id, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << id << endl;
}

#ifndef DEEPSLEEP
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
    if ((timerSwitchValue == 0 ? false : true) != digitalRead(SWITCH_BUTTON))
    {
      digitalWrite(SWITCH_BUTTON, !digitalRead(SWITCH_BUTTON));
      writeSwitchStateEEPROM();
      Serial << "switch:                ";
      timerSwitchValue == 0 ? Serial << "off" << endl : Serial << "on" << endl;
    }
  }
  else
  {
    digitalWrite(SWITCH_BUTTON, 1);
    writeSwitchStateEEPROM();
    timerSwitchStart = millis();
    Serial << "switch:                on" << endl;
  }
}
#endif

void setupMqttTopic()
{
  if (String(mqtt_server).length() != 0)
  {
    Serial << "mqtt publish topic:    ";
    mqttTopicPublish = id + String(mqtt_value_prefix);
    Serial << mqttTopicPublish << endl;
    Serial << "mqtt subcribe topic:   ";
    mqttTopicSubscribe = id + String(mqtt_switch_prefix);
    Serial << mqttTopicSubscribe << endl;
  }
}

void setupMqttServer()
{
  if (String(mqtt_server).length() != 0)
  {
    Serial << "mqtt server:           " << mqtt_server << endl;
    Serial << "mqtt port:             " << mqtt_port << endl;

    pubSubClient.setClient(wifiClient);
    pubSubClient.setServer(mqtt_server, String(mqtt_port).toInt());

#ifndef DEEPSLEEP
    pubSubClient.setCallback(callback);
#endif
  }
}

void connectMqtt()
{
  if (String(mqtt_server).length() != 0)
  {
    Serial << "mqtt:                  ";
    Serial << "connecting ... ";

    if (WiFi.getMode() == WIFI_AP or WiFi.status() == WL_CONNECTED)
    {
      if (!pubSubClient.connected())
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
        if (pubSubClient.connected())
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
    }
    else
    {
      Serial << "not connected (no wifi)" << endl;
    }
  }
}

void publishMqtt(String value)
{
  if (String(mqtt_server).length() != 0)
  {
    boolean published = false;

    Serial << "mqtt:                  ";
    Serial << "publishing ... ";

    if (WiFi.getMode() == WIFI_AP or WiFi.status() == WL_CONNECTED)
    {
      if (pubSubClient.connected())
        published = pubSubClient.publish(mqttTopicPublish.c_str(), value.c_str());
      else
      {
        Serial << "not connected" << endl;
        connectMqtt();
        Serial << "mqtt:                  ";
        Serial << "publishing ... ";
        published = pubSubClient.publish(mqttTopicPublish.c_str(), value.c_str());
      }
      if (published)
        Serial << "published" << endl;
      else
        Serial << "not published" << endl;
    }
    else
      Serial << "not published (no wifi)" << endl;
  }
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_BUTTON, OUTPUT);
  setLED();

  Serial.begin(115200);
  Serial.setDebugOutput(false);
  Wire.begin();

  printVersion();
  setupSensors();
  readSwitchStateEEPROM();

#ifdef DEEPSLEEP
  setupWiFi(WIFI_STA);
#else
  setupWiFi(WIFI_MODE);
#endif
  connectWiFi();
  setLED();

#ifndef DEEPSLEEP
  setupOTA();
  setupWebServer();
#endif

  setupID();
  setupMqttTopic();
  setupMqttServer();
  connectMqtt();
}

void loop()
{
  setLED();

  if (WiFi.getMode() == WIFI_AP or wifiMulti.run() == WL_CONNECTED)
  {
#ifndef DEEPSLEEP
    ArduinoOTA.handle();
    server.handleClient();
#endif
    pubSubClient.loop();
  }

  if (millis() - timerMeasureIntervallStart > timerMeasureIntervall * 1000)
  {
    timerMeasureIntervallStart = millis();
    value = getValue();
    showValue(value);
    publishMqtt(value);
#ifdef DEEPSLEEP
    Serial << "deepsleep:    " << DEEPSLEEP << " sec" << endl;
    yield();
    ESP.deepSleep(DEEPSLEEP * 1000000);
#endif
  }

#ifndef DEEPSLEEP
  if (timerSwitchValue > 1 and digitalRead(SWITCH_BUTTON) == 1 and millis() - timerSwitchStart > timerSwitchValue)
  {
    digitalWrite(SWITCH_BUTTON, 0);
    writeSwitchStateEEPROM();
    Serial << "switch:                off" << endl;
  }
#endif
}
