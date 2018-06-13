#include <BMP180.h>

BMP180::BMP180(int sensorAddress, int sensorID) {
  this->sensorAddress = sensorAddress;
  this->sensorID = sensorID;
}

bool BMP180::begin() {
  if (checkAddress()) {
    writeDataByte(CHIP_ID_REGISTER);
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

}

void BMP180::readUncompensatedTemperature() {

}

void BMP180::readUncompensatedPressure() {

}

float BMP180::calculateTrueTemperature() {
  return 0.0;
}

float BMP180::calculateTruePressure() {
  return 0.0;
}
