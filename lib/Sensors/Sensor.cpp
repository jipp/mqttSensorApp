#include <Sensor.h>

bool Sensor::checkAddress() {
  byte transmissionStatus;

  Wire.beginTransmission(this->sensorAddress);
  transmissionStatus = Wire.endTransmission();

  return transmissionStatus == 0 ? true : false;
}

void Sensor::writeData(byte data) {
  Wire.beginTransmission(this->sensorAddress);
  Wire.write(data);
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
