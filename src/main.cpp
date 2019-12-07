#include <Arduino.h>

#include <iostream>

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
  mqttClient.publish("test/lol", 0, true, "test 1");
  std::cout << "Publishing at QoS 0" << std::endl;
  uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
  std::cout << "Publishing at QoS 1, packetId: " << packetIdPub1 << std::endl;
  uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
  std::cout << "Publishing at QoS 2, packetId: " << packetIdPub2 << std::endl;
}

void onMqttConnect(bool sessionPresent)
{
  std::cout << "Connected to MQTT." << std::endl;
  std::cout << "Session present: " << sessionPresent << std::endl;
  uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
  std::cout << "Subscribing at QoS 2, packetId: " << packetIdSub << std::endl;
  mqttPublish.attach(10, mqttPublishMessage);
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
  std::cout << "  qos: " << qos << std::endl;
}

void onMqttUnsubscribe(uint16_t packetId)
{
  std::cout << "Unsubscribe acknowledged." << std::endl;
  std::cout << "  packetId: " << packetId << std::endl;
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  std::cout << "Publish received." << std::endl;
  std::cout << "  topic: " << topic << std::endl;
  std::cout << "  qos: " << properties.qos << std::endl;
  std::cout << "  dup: " << properties.dup << std::endl;
  std::cout << "  retain: " << properties.retain << std::endl;
  std::cout << "  len: " << len << std::endl;
  std::cout << "  index: " << index << std::endl;
  std::cout << "  total: " << total << std::endl;
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