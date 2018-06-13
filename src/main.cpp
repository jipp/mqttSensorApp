#include <Arduino.h>

#include "config.h"

#include <Streaming.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>

#include <VCC.h>
#include <BH1750.h>
#include <SHT3X.h>
#include <BMP180.h>


ADC_MODE(ADC_VCC);


ESP8266WiFiMulti wifiMulti;
PubSubClient pubSubClient;
WiFiClient wifiClient;

VCC vcc = VCC();
BH1750 bh1750 = BH1750();
SHT3X sht3x = SHT3X(0x45);
BMP180 bmp180 = BMP180();

char id[13];
String publishTopic;
unsigned long timerMeasureIntervallStart = 0;
unsigned long timerLastReconnectStart = 0;


void printSettings() {
  Serial << endl << endl << "Reset Info: " << ESP.getResetInfo() << endl;
  Serial << endl << "VERSION: " << VERSION << endl;
  #ifdef DEEPSLEEP
  Serial << "DEEPSLEEP: " << DEEPSLEEP << " s" << endl;
  #endif
  Serial << "TIMER: " << TIMER << "s" << endl;
  Serial << "IP: " << WiFi.localIP() << endl;
  Serial << "id: " << id << endl;
  Serial << "mqtt Server: " << mqtt_server << endl;
  Serial << "mqtt Port: " << mqtt_port << endl;
  Serial << "mqtt Username: " << mqtt_username << endl;
  Serial << "topic: " << publishTopic << endl;
  Serial << endl;
}

void connectWiFi() {
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial << ".";
    delay(100);
  }
  Serial << " connected!" << endl;
}

void setupWiFi() {
  Serial << endl << "connecting:" << endl;
  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);
  connectWiFi();
}

void setupID() {
  byte mac[6];

  WiFi.macAddress(mac);
  sprintf(id, "%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void setupTopic() {
  publishTopic = id + String(mqtt_topic_prefix);
}

void setupPubSub() {
  pubSubClient.setClient(wifiClient);
  pubSubClient.setServer(mqtt_server, String(mqtt_port).toInt());
}

void publishValues() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  JsonArray& vccJson = json.createNestedArray("vcc");
  JsonArray& illuminanceJson = json.createNestedArray("illuminance");
  JsonArray& temperatureJson = json.createNestedArray("temperature");
  JsonArray& humidityJson = json.createNestedArray("humidity");
  JsonArray& pressureJson = json.createNestedArray("pressure");
  String jsonString;

  if (vcc.isAvailable) {
    vccJson.add(vcc.getVCC());
  }
  if (bh1750.isAvailable) {
    illuminanceJson.add(bh1750.getIlluminance());
  }
  if (sht3x.isAvailable) {
    sht3x.getValues();
    temperatureJson.add(sht3x.temperature);
    humidityJson.add(sht3x.humidity);
  }
  if (bmp180.isAvailable()) {
    bmp180.getValue();
    temperatureJson.add(bmp180.temperature);
    humidityJson.add(bmp180.humidity);
    pressureJson.add(bmp180.pressure);
  }

  if (pubSubClient.connected()) {
    jsonString = "";
    json.printTo(jsonString);
    if (pubSubClient.publish(publishTopic.c_str(), jsonString.c_str())) {
      Serial << " < " << publishTopic << ": " << jsonString << endl;
    }
  }
}

bool connect() {
  bool connected = false;

  Serial << "Attempting MQTT connection (~5s) ..." << endl;
  if ((String(mqtt_username).length() == 0) || (String(mqtt_password).length() == 0)) {
    Serial << "trying without MQTT authentication" << endl;
    connected = pubSubClient.connect(id);
  } else {
    Serial << "trying with MQTT authentication" << endl;
    connected = pubSubClient.connect(id, String(mqtt_username).c_str(), String(mqtt_password).c_str());
  }
  if (connected) {
    Serial << "MQTT connected, rc=" << pubSubClient.state() << endl;
  } else {
    Serial << "MQTT failed, rc=" << pubSubClient.state() << endl;
  }

  return pubSubClient.connected();
}


void setup() {
  Serial.begin(115200);
  Wire.begin();

  Serial.setDebugOutput(false);

  setupWiFi();
  setupID();
  setupTopic();
  printSettings();
  setupPubSub();
  vcc.begin();
  bh1750.begin();
  sht3x.begin();
  if (connect()) {
    publishValues();
  }
}

void loop() {
  if (wifiMulti.run() == WL_CONNECTED) {
    if (!pubSubClient.connected()) {
      if (millis() - timerLastReconnectStart > timerLastReconnect * 1000) {
        timerLastReconnectStart = millis();
        if (connect()) {
          timerLastReconnectStart = 0;
        }
      }
    } else {
      pubSubClient.loop();
      if (millis() - timerMeasureIntervallStart > timerMeasureIntervall * 1000) {
        timerMeasureIntervallStart = millis();
        publishValues();
      }
    }
  } else {
    connectWiFi();
  }
}
