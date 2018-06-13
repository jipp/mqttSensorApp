#include <BH1750.h>

BH1750::BH1750(int sensorAddress) {
  this->sensorAddress = sensorAddress;
}

bool BH1750::begin() {
  this->isAvailable = checkAddress();

  return this->isAvailable;
}

uint16_t BH1750::getIlluminance() {
  writeDataByte(CONTINUOUSLY_H_RESOLUTION_MODE);
  delayMicroseconds(120);

  return readDataInt();
}
