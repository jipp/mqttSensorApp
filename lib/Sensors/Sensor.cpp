#include <Sensor.h>

bool Sensor::checkSensorAvailability(byte sensorAddress) {
  byte transmissionStatus;

  Wire.beginTransmission(this->sensorAddress);
  transmissionStatus = Wire.endTransmission();

  return transmissionStatus == 0 ? true : false;
}

bool Sensor::checkAddress() {
  byte transmissionStatus;

  Wire.beginTransmission(this->sensorAddress);
  transmissionStatus = Wire.endTransmission();

  return transmissionStatus == 0 ? true : false;
}

void Sensor::writeDataByte(byte data) {
  Wire.beginTransmission(this->sensorAddress);
  Wire.write(data);
  Wire.endTransmission();
}

void Sensor::writeDataInt(uint16_t data) {
  Wire.beginTransmission(this->sensorAddress);
  Wire.write(data >> 8);
  Wire.write(data & 0xFF);
  Wire.endTransmission();
}

byte Sensor::readDataByte() {
  byte data = 0;

  Wire.requestFrom(this->sensorAddress, 1);
  if (Wire.available() == 1) {
    data = Wire.read();
  }

  return data;
}

uint16_t Sensor::readDataInt() {
  uint16_t data = 0;

  Wire.requestFrom(this->sensorAddress, 2);
  if (Wire.available() == 2) {
    data = Wire.read();
    data <<= 8;
    data |= Wire.read();
  }

  return data;
}

void Sensor::writeRegisterByte(byte data, byte registerAddress) {
  Wire.beginTransmission(this->sensorAddress);
  Wire.write(data);
  Wire.write(registerAddress);
  Wire.endTransmission();
}

byte Sensor::readRegisterByte(byte registerAddress) {
  byte data = 0;

  writeDataByte(registerAddress);
  Wire.beginTransmission(this->sensorAddress);
  Wire.requestFrom(this->sensorAddress, 1);
  if (Wire.available() == 1) {
    data = Wire.read();
  }
  Wire.endTransmission();

  return data;
}

uint16_t Sensor::readRegisterInt(byte registerAddress) {
  uint16_t data = 0;

  writeDataByte(registerAddress);
  Wire.beginTransmission(this->sensorAddress);
  Wire.requestFrom(this->sensorAddress, 2);
  if (Wire.available() == 2) {
    data = Wire.read();
    data <<= 8;
    data |= Wire.read();
  }
  Wire.endTransmission();

  return data;
}
