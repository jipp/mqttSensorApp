#include <Arduino.h>

#include "config.h"

#include <Streaming.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <FS.h>

#include <i2cSensorLib.h>


ADC_MODE(ADC_VCC);


ESP8266WiFiMulti wifiMulti;
PubSubClient pubSubClient;
WiFiClient wifiClient;
ESP8266WebServer server(80);

VCC vcc = VCC();
BH1750 bh1750 = BH1750();
SHT3X sht3x = SHT3X(0x45);
BMP180 bmp180 = BMP180();
BME280 bme280 = BME280();

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

void setupFS() {
  File f;

  SPIFFS.begin();

  f = SPIFFS.open(VERSION, "w");
  f.close();

  if (SPIFFS.exists(VERSION)) {
    SPIFFS.remove(VERSION);
  } else {
    Serial << "format - this can take several minutes" << endl;
    SPIFFS.format();
    Serial << "done" << endl;
  }
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void writeData(String string) {
  File dataTempHandler, dataHandler;

  dataTempHandler = SPIFFS.open(dataTemp, "a");
  dataTempHandler.println(string);

  dataHandler = SPIFFS.open(data, "r");
  for (int i=1; i<bufferLength; i++) {
    if (dataHandler.available()) {
      String line = dataHandler.readStringUntil('\n');
      dataTempHandler.println(line);
    } else {
      break;
    }
  }

  dataHandler.close();
  dataTempHandler.close();

  SPIFFS.remove(data);
  SPIFFS.rename(dataTemp, data);
}

void readData() {
  File f;

  Serial.println();
  f = SPIFFS.open("/data.txt", "r");
  while (f.available()) {
    String line = f.readStringUntil('\n');
    Serial.println(line);
  }
  f.close();
  Serial.println();
}

String readDataLine() {
  File f;
  String line;

  f = SPIFFS.open("/data.txt", "r");
  if (f.available()) {
    line = f.readStringUntil('\n');
  }
  f.close();

  return line;
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

void setupMDNS() {
  if (MDNS.begin(id)) {
    Serial.println("MDNS responder started");
  }
}

void setupWebServer() {
  server.on("/value", []() {
    server.send(200, "application/json", readDataLine());
  });
  server.serveStatic("/", SPIFFS, "/");
  server.onNotFound(handleNotFound);
  server.begin();
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
    vcc.getValues();
    vccJson.add(vcc.get(Sensor::VOLTAGE_MEASUREMENT));
  }
  if (bh1750.isAvailable) {
    bh1750.getValues();
    illuminanceJson.add(bh1750.get(Sensor::ILLUMINANCE_MEASUREMENT));
  }
  if (sht3x.isAvailable) {
    sht3x.getValues();
    temperatureJson.add(sht3x.get(Sensor::TEMPERATURE_MEASUREMENT));
    humidityJson.add(sht3x.get(Sensor::HUMIDITY_MEASUREMENT));
  }
  if (bmp180.isAvailable) {
    bmp180.getValues();
    temperatureJson.add(bmp180.get(Sensor::TEMPERATURE_MEASUREMENT));
    pressureJson.add(bmp180.get(Sensor::PRESSURE_MEASUREMENT));
  }
  if (bme280.isAvailable) {
    bme280.getValues();
    temperatureJson.add(bme280.get(Sensor::TEMPERATURE_MEASUREMENT));
    pressureJson.add(bme280.get(Sensor::PRESSURE_MEASUREMENT));
    humidityJson.add(bme280.get(Sensor::HUMIDITY_MEASUREMENT));
  }

  if (pubSubClient.connected()) {
    jsonString = "";
    json.printTo(jsonString);
    if (pubSubClient.publish(publishTopic.c_str(), jsonString.c_str())) {
      Serial << " < " << publishTopic << ": " << jsonString << endl;
    }
  }

  writeData(jsonString);
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

  setupFS();
  setupWiFi();
  setupID();
  setupMDNS();
  setupWebServer();
  setupTopic();
  printSettings();
  setupPubSub();
  vcc.begin();
  bh1750.begin();
  sht3x.begin();
  bmp180.begin();
  bme280.begin();
  if (connect()) {
    publishValues();
  }
}

void loop() {
  server.handleClient();
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
