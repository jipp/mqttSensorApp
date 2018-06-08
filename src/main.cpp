#include <Arduino.h>

#include "config.h"

#include <Streaming.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

ESP8266WiFiMulti wifiMulti;
PubSubClient pubSubClient;
WiFiClient wifiClient;

char id[13];
String publishTopic;
unsigned long timerMeasureIntervallStart = 0;
unsigned long timerLastReconnectStart = 0;


ADC_MODE(ADC_VCC);


void printSettings() {
  Serial << endl << endl << "RESETINFO: " << ESP.getResetInfo() << endl;
  Serial << endl << "VERSION: " << VERSION << endl;
  #ifdef DEEPSLEEP
  Serial << "DEEPSLEEP: " << DEEPSLEEP << " s" << endl;
  #endif
  Serial << "TIMER: " << TIMER << " s" << endl;
  Serial << "id: " << id << endl;
  Serial << "mqtt Server: " << mqtt_server << endl;
  Serial << "mqtt Username: " << mqtt_username << endl;
  Serial << "topic: " << publishTopic << endl;
  Serial << endl;
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
  Serial << millis() << endl;
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
  Serial.setDebugOutput(false);

  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);

  setupID();
  setupTopic();
  printSettings();
  setupPubSub();
  if (connect()) {
    publishValues();
  }
}

void loop() {
  wifiMulti.run();
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
}
