#include <BMP180.h>

BMP180::BMP180(byte sensorAddress, byte sensorIDRegister, byte sensorID) {
  this->sensorAddress = sensorAddress;
  this->sensorIDRegister = sensorIDRegister;
  this->sensorID = sensorID;
}

void BMP180::begin() {
  this->isAvailable = this->checkSensorAvailability(this->sensorAddress, this->sensorIDRegister, this->sensorID);
  this->readCalibrationData();
}

void BMP180::getValues() {
  this->readUncompensatedTemperature();
  this->readUncompensatedPressure();
  this->temperature = calculateTrueTemperature();
  this->pressure = calculateTruePressure();
}

void BMP180::wait(uint16_t Mode) {
  switch(Mode) {
    case ULTRA_LOW_POWER:
    delayMicroseconds(450);
    break;
    case STANDARD:
    delayMicroseconds(750);
    break;
    case HIGH_RESOLUTION:
    delayMicroseconds(13500);
    break;
    case ULTRA_HIGH_RESOLUTION:
    delayMicroseconds(25500);
    break;
  };
}

void BMP180::readCalibrationData() {
  this->calibrationCoefficients.ac1 = readRegister16(this->sensorAddress, REGISTER_AC1);
  this->calibrationCoefficients.ac2 = readRegister16(this->sensorAddress, REGISTER_AC2);
  this->calibrationCoefficients.ac3 = readRegister16(this->sensorAddress, REGISTER_AC3);
  this->calibrationCoefficients.ac4 = readRegister16(this->sensorAddress, REGISTER_AC4);
  this->calibrationCoefficients.ac5 = readRegister16(this->sensorAddress, REGISTER_AC5);
  this->calibrationCoefficients.ac6 = readRegister16(this->sensorAddress, REGISTER_AC6);
  this->calibrationCoefficients.b1 = readRegister16(this->sensorAddress, REGISTER_B1);
  this->calibrationCoefficients.b2 = readRegister16(this->sensorAddress, REGISTER_B2);
  this->calibrationCoefficients.mb = readRegister16(this->sensorAddress, REGISTER_MB);
  this->calibrationCoefficients.mc = readRegister16(this->sensorAddress, REGISTER_MC);
  this->calibrationCoefficients.md = readRegister16(this->sensorAddress, REGISTER_MD);
}

void BMP180::readUncompensatedTemperature() {
  byte MSB;
  byte LSB;

  writeRegister8(this->sensorAddress, 0xF4, 0x2E);
  delayMicroseconds(4500);

  MSB = readRegister8(this->sensorAddress, 0xF6);
  LSB = readRegister8(this->sensorAddress, 0xF7);

  this->UT = (MSB << 8) + LSB;
}

void BMP180::readUncompensatedPressure() {
  byte MSB;
  byte LSB;
  byte XLSB;

  writeRegister8(this->sensorAddress, 0xF4, PRESSURE_OSS0 | (BMP180_MODE << 6));
  wait(BMP180_MODE);

  MSB = readRegister8(this->sensorAddress, 0xF6);
  LSB = readRegister8(this->sensorAddress, 0xF7);
  XLSB = readRegister8(this->sensorAddress, 0xF8);

  this->UP = ((MSB << 16) | (LSB << 8) | XLSB) >> (8 - BMP180_MODE);
}

float BMP180::calculateTrueTemperature() {
  int32_t X1, X2, T;

  X1 = (this->UT - this->calibrationCoefficients.ac6) * this->calibrationCoefficients.ac5 >> 15;
  X2 = (this->calibrationCoefficients.mc << 11) / (X1 + this->calibrationCoefficients.md);
  this->B5 = X1 + X2;
  T = (this->B5 + 8) >> 4;

  return T / 10.0;
}

float BMP180::calculateTruePressure() {
  int32_t B6, X1, X2, X3, B3, p;
  uint32_t B4, B7;

  B6 = this->B5 - 4000;
  X1 = (calibrationCoefficients.b2 * (B6 * B6 >> 12)) >> 11;
  X2 = calibrationCoefficients.ac2 * B6 >> 11;
  X3 = X1 + X2;
  B3 = (((calibrationCoefficients.ac1 * 4 + X3) << BMP180_MODE) + 2) / 4;
  X1 = calibrationCoefficients.ac3 * B6 >> 13;
  X2 = (calibrationCoefficients.b1 * (B6 * B6 >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = calibrationCoefficients.ac4 * (uint32_t)(X3 + 32768) >> 15;
  B7 = ((uint32_t)this->UP - B3) * (50000 >> BMP180_MODE);
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
