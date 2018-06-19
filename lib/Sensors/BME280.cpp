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
      this->readCalibrationData();
      return true;
    }
  }

  return false;
}

void BME280::getValues() {
  this->readUncompensatedTemperature();
  this->readUncompensatedPressure();

  this->temperature = this->calculateTrueTemperature();
  this->pressure = this->calculateTruePressure();
}

void BME280::readCalibrationData() {
  this->calibrationCoefficients.ac1 = readRegisterInt(REGISTER_AC1);
  this->calibrationCoefficients.ac2 = readRegisterInt(REGISTER_AC2);
  this->calibrationCoefficients.ac3 = readRegisterInt(REGISTER_AC3);
  this->calibrationCoefficients.ac4 = readRegisterInt(REGISTER_AC4);
  this->calibrationCoefficients.ac5 = readRegisterInt(REGISTER_AC5);
  this->calibrationCoefficients.ac6 = readRegisterInt(REGISTER_AC6);
  this->calibrationCoefficients.b1 = readRegisterInt(REGISTER_B1);
  this->calibrationCoefficients.b2 = readRegisterInt(REGISTER_B2);
  this->calibrationCoefficients.mb = readRegisterInt(REGISTER_MB);
  this->calibrationCoefficients.mc = readRegisterInt(REGISTER_MC);
  this->calibrationCoefficients.md = readRegisterInt(REGISTER_MD);
}

void BME280::readUncompensatedTemperature() {
  byte MSB;
  byte LSB;

  writeRegisterByte(0xF4, 0x2E);

  delayMicroseconds(4500);

  MSB = readRegisterByte(0xF6);
  LSB = readRegisterByte(0xF7);

  this->UT = (MSB << 8) + LSB;
}

void BME280::readUncompensatedPressure() {
  byte MSB;
  byte LSB;
  byte XLSB;

  writeRegisterByte(0xF4, 0x34 | (STANDARD << 6));

  delayMicroseconds(7500);

  MSB = readRegisterByte(0xF6);
  LSB = readRegisterByte(0xF7);
  XLSB = readRegisterByte(0xF8);

  this->UP = ((MSB << 16) | (LSB << 8) | XLSB) >> (8 - STANDARD);
}

float BME280::calculateTrueTemperature() {
  int32_t X1, X2, T;

  X1 = (this->UT - this->calibrationCoefficients.ac6) * this->calibrationCoefficients.ac5 >> 15;
  X2 = (this->calibrationCoefficients.mc << 11) / (X1 + this->calibrationCoefficients.md);
  this->B5 = X1 + X2;
  T = (this->B5 + 8) >> 4;

  return T / 10.0;
}

float BME280::calculateTruePressure() {
  int32_t B6, X1, X2, X3, B3, p;
  uint32_t B4, B7;

  B6 = this->B5 - 4000;
  X1 = (calibrationCoefficients.b2 * (B6 * B6 >> 12)) >> 11;
  X2 = calibrationCoefficients.ac2 * B6 >> 11;
  X3 = X1 + X2;
  B3 = (((calibrationCoefficients.ac1 * 4 + X3) << STANDARD) + 2) / 4;
  X1 = calibrationCoefficients.ac3 * B6 >> 13;
  X2 = (calibrationCoefficients.b1 * (B6 * B6 >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = calibrationCoefficients.ac4 * (uint32_t)(X3 + 32768) >> 15;
  B7 = ((uint32_t)this->UP - B3) * (50000 >> STANDARD);
  if (B7 < 0x80000000) {
    p = (B7 * 2) / B4;
  } else {
    p = (B7 / B4) * 2;
  }
  X1 = (p >> 8) * (p >> 8);
  X1 = (X1 * 3038) >> 16;
  X2 = (-7357 * p) >> 16;
  p = p + ((X1 + X2 + 3791) >> 4);

  return p / 100.0;
}