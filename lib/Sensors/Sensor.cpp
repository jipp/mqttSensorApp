#include <Sensor.h>

bool Sensor::checkSensorAvailability(byte sensorAddress) {
  byte transmissionStatus;

  Wire.beginTransmission(sensorAddress);
  transmissionStatus = Wire.endTransmission();

  return transmissionStatus == 0 ? true : false;
}

bool Sensor::checkSensorAvailability(byte sensorAddress, byte registerAddress, byte value) {
  if (this->checkSensorAvailability(sensorAddress)) {
    if (value == this->readRegister8(sensorAddress, registerAddress)) {
      return true;
    }
  };

  return false;
}

void Sensor::writeRegister8(byte sensorAddress, byte registerAddress) {
  Wire.beginTransmission(sensorAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
}

void Sensor::writeRegister8(byte sensorAddress, byte registerAddress, byte registerValue) {
  Wire.beginTransmission(sensorAddress);
  Wire.write(registerAddress);
  Wire.write(registerValue);
  Wire.endTransmission();
}

void Sensor::writeRegister16(byte sensorAddress, uint16_t registerAddress) {
  Wire.beginTransmission(sensorAddress);
  Wire.write(registerAddress >> 8);
  Wire.write(registerAddress & 0xFF);
  Wire.endTransmission();
}

byte Sensor::readRegister8(byte sensorAddress) {
  byte registerValue = 0x00;

  Wire.requestFrom((int) sensorAddress, 1);
  if (Wire.available() == 1) {
    registerValue = Wire.read();
  }

  return registerValue;
}

byte Sensor::readRegister8(byte sensorAddress, byte registerAddress) {
  byte registerValue = 0x00;

  Wire.beginTransmission(sensorAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom((int) sensorAddress, 1);
  if (Wire.available() == 1) {
    registerValue = Wire.read();
  }
  Wire.endTransmission();

  return registerValue;
}

uint16_t Sensor::readRegister16(byte sensorAddress) {
  uint16_t registerValue = 0;

  Wire.requestFrom((int) sensorAddress, 2);
  if (Wire.available() == 2) {
    registerValue = Wire.read();
    registerValue <<= 8;
    registerValue |= Wire.read();
  }

  return registerValue;
}

uint16_t Sensor::readRegister16(byte sensorAddress, byte registerAddress) {
  uint16_t registerValue = 0x00;

  Wire.beginTransmission(sensorAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom((int) sensorAddress, 2);
  if (Wire.available() == 2) {
    registerValue = Wire.read();
    registerValue <<= 8;
    registerValue |= Wire.read();
  }

  return registerValue;
}

uint32_t Sensor::readRegister24(byte sensorAddress, byte registerAddress) {
  uint32_t registerValue = 0x00;

  Wire.beginTransmission(sensorAddress);
  Wire.write(registerAddress);
  Wire.endTransmission();
  Wire.requestFrom((int) sensorAddress, 3);
  if (Wire.available() == 3) {
    registerValue = Wire.read();
    registerValue <<= 8;
    registerValue |= Wire.read();
    registerValue <<= 8;
    registerValue |= Wire.read();
  }

  return registerValue;
}
