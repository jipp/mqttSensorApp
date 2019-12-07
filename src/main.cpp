#include <Arduino.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include <AsyncMqttClient.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <WiFiManager.h>

#include "config.hpp"

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
  mqttPublish.detach();
  mqttReconnect.detach();
  wifiReconnect.once(2, wifiConnect);
}

void onGotIp(const WiFiEventStationModeGotIP &event)
{
  std::cout << "Got IP: " << std::string(WiFi.localIP().toString().c_str()) << std::endl;
  connectToMqtt();
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
  uint16_t packetIdPub = mqttClient.publish(mqttPublishTopic.c_str(), mqttPublishQoS, true, "test");
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

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  std::string buffer;

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
}

void onMqttPublish(uint16_t packetId)
{
  std::cout << "Publish acknowledged." << std::endl;
  std::cout << "  packetId: " << packetId << std::endl;
}

void setup()
{
  // general setup
  Serial.begin(460800);

  // setup WiFi
  connectedEventHandler = WiFi.onStationModeConnected(onConnected);
  disconnectedEventHandler = WiFi.onStationModeDisconnected(onDisconnected);
  gotIpEventHandler = WiFi.onStationModeGotIP(onGotIp);
  DHCPTimeoutHandler = WiFi.onStationModeDHCPTimeout(onDHCPTimeout);

  // setup mqtt
  byte mac[6];
  std::ostringstream stringStream;
  WiFi.macAddress(mac);
  stringStream << std::hex << std::setw(2) << std::setfill('0') << (int)mac[0]
               << std::hex << std::setw(2) << std::setfill('0') << (int)mac[1]
               << std::hex << std::setw(2) << std::setfill('0') << (int)mac[2]
               << std::hex << std::setw(2) << std::setfill('0') << (int)mac[3]
               << std::hex << std::setw(2) << std::setfill('0') << (int)mac[4]
               << std::hex << std::setw(2) << std::setfill('0') << (int)mac[5];
  id = stringStream.str();
  std::cout << "id: " << id << std::endl;
  mqttPublishTopic = id + mqttValuePrefix;
  mqttSubscribeTopic = id + mqttSwitchPrefix;

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.setServer(mqttServer.c_str(), mqttPort);
  mqttClient.setCredentials(mqttUsername.c_str(), mqttPassword.c_str());
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);

  // start
  getWiFi();
}

void loop()
{
}