#include "Sensor.h"

Sensor::Sensor(byte sensorAddr) {
  this->sensorAddr = sensorAddr;
}

bool Sensor::getIsAvailable() {
  return this->isAvailable;
}

float Sensor::getVCC() {
  return this->vcc;
}

float Sensor::getTemperature() {
  return this->temperature;
}

float Sensor::getHumidity() {
  return this->temperature;
}

float Sensor::getPressure() {
  return this->temperature;
}

float Sensor::getIlluminance() {
  return this->illuminance;
}

void Sensor::getValues() {
  this->temperature = 0.0;
  this->humidity = 0.0;
  this->pressure = 0.0;
  this->illuminance = 0.0;
}
