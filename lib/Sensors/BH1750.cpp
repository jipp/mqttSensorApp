#include <BH1750.h>

BH1750::BH1750(byte sensorAddress) {
  this->sensorAddress = sensorAddress;
}

void BH1750::begin() {
  this->isAvailable = this->checkSensorAvailability(this->sensorAddress);
}

void BH1750::getValues() {
  writeRegister8(this->sensorAddress, BH1750_MODE);
  wait(BH1750_MODE);

  this->illuminance = readRegister16(this->sensorAddress);
}

void BH1750::wait(uint16_t Mode) {
  switch(Mode) {
    case CONTINUOUSLY_H_RESOLUTION_MODE:
    delayMicroseconds(120);
    break;
    case CONTINUOUSLY_H_RESOLUTION_MODE2:
    delayMicroseconds(120);
    break;
    case CONTINUOUSLY_L_RESOLUTION_MODE:
    delayMicroseconds(16);
    break;
    case ONE_TIME_H_RESOLUTION_MODE:
    delayMicroseconds(120);
    break;
    case ONE_TIME_H_RESOLUTION_MODE2:
    delayMicroseconds(120);
    break;
    case ONE_TIME_L_RESOLUTION_MODE:
    delayMicroseconds(16);
    break;
  };
}
