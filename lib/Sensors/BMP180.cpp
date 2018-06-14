#include <BMP180.h>

BMP180::BMP180(int sensorAddress, int sensorID) {
  this->sensorAddress = sensorAddress;
  this->sensorID = sensorID;
}

bool BMP180::begin() {
  if (checkAddress()) {
    writeDataByte(0xD0);
    if (readDataByte() == this->sensorID) {
      this->isAvailable = true;
      this->readCalibrationData();
      return true;
    }
  }

  return false;
}

void BMP180::getValues() {
  this->readUncompensatedTemperature();
  this->readUncompensatedPressure();

  this->temperature = this->calculateTrueTemperature();
  this->pressure = this->calculateTruePressure();
}

void BMP180::readCalibrationData() {
  writeDataByte(AC1);
  this->ac1 = readDataInt();
  writeDataByte(AC2);
  this->ac2 = readDataInt();
  writeDataByte(AC3);
  this->ac3 = readDataInt();
  writeDataByte(AC4);
  this->ac4 = readDataInt();
  writeDataByte(AC5);
  this->ac5 = readDataInt();
  writeDataByte(AC6);
  this->ac6 = readDataInt();

  writeDataByte(_B1);
  this->b1 = readDataInt();
  writeDataByte(B2);
  this->b2 = readDataInt();

  writeDataByte(MB);
  this->mb = readDataInt();
  writeDataByte(MC);
  this->mc = readDataInt();
  writeDataByte(MD);
  this->md = readDataInt();
}

void BMP180::readUncompensatedTemperature() {
  int MSB;
  int LSB;

  writeDataByte(0xF4);
  writeDataByte(0x2E);

  delayMicroseconds(4500);

  writeDataByte(0xF6);
  MSB = readDataByte();
  writeDataByte(0xF7);
  LSB = readDataByte();
  this->UT = (MSB << 8) + LSB;
}

void BMP180::readUncompensatedPressure() {
  int MSB;
  int LSB;
  int XLSB;

  writeDataByte(0xF4);
  writeDataByte(0x34 | (STANDARD << 6));
  writeDataByte(0xF6);
  MSB = readDataByte();
  writeDataByte(0xF7);
  LSB = readDataByte();
  writeDataByte(0xF8);
  XLSB = readDataByte();

  this->UP = ((MSB << 16) | (LSB << 8) | XLSB) >> (8 - STANDARD);
}

float BMP180::calculateTrueTemperature() {
  uint16_t X1, X2;

  X1 = (this->UT - AC6) * AC5 >> 15;
  X2 = ( MC << 11 ) / ( X1 + MD );
  this->B5 = X1 + X2;

  return ( this->B5 + 8 ) >> 4;
}

float BMP180::calculateTruePressure() {
  uint16_t B6, X1, X2, X3, B3, B4, B7;
  uint32_t p;

  B6 = this->B5 - 4000;
  X1 = ( b2 * ( B6 * B6 >> 12)) >> 11;
  X3 = ac2 * B6 >> 11;
  B3 = ac4 * (uint32_t) (X3 + 32768) >> 15;
  B4 = ac4 * (uint32_t) (X3 + 32768) >> 15;
  B7 = ((uint32_t) (this->UP - B3)) * (5000 >> STANDARD);
  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) *2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;
p = p + ((X1 + X2+ 3791) >> 4);

return p;
}
