#include <BH1750.h>

BH1750::BH1750(int sensorAddress) {
  this->sensorAddress = sensorAddress;
}

bool BH1750::begin() {
  this->isAvailable = checkAddress();

  return this->isAvailable;
}

uint16_t BH1750::getIlluminance() {
  writeDataByte(BH1750_INSTRUCTION);
  wait(BH1750_INSTRUCTION);

  return readDataInt();
}

void BH1750::wait(uint16_t Instruction) {
  switch(Instruction) {
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
