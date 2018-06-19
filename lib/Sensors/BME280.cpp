#include <BME280.h>

BME280::BME280(int sensorAddress, int sensorID) {
  this->sensorAddress = sensorAddress;
  this->sensorID = sensorID;
}

bool BME280::begin() {
  if (checkAddress()) {
    writeDataByte(0xD0);
    if (readDataByte() == this->sensorID) {
      this->isAvailable = true;
      return true;
    }
  }

  return false;
}

void BME280::getValues() {

  this->temperature = 0.0;
  this->pressure = 0.0;
  this->humidity = 0.0;
}
