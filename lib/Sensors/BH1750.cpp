#include "BH1750.h"

BH1750::BH1750(byte addr) {
  BH1750_I2CADDR = addr;
}

bool BH1750::isAvailable() {
  bool isAvailable = false;

  Wire.beginTransmission(BH1750_I2CADDR);
  isAvailable = Wire.endTransmission();

  return isAvailable == 0 ? true : false;
}

int BH1750::getValue() {
  uint16_t value = 65535;

  Wire.beginTransmission(BH1750_I2CADDR);
  Wire.write(CONTINUOUS_HIGH_RES_MODE);
  Wire.endTransmission();

  delayMicroseconds(120);
  Wire.requestFrom(BH1750_I2CADDR, 2);
  if (Wire.available() == 2) {
    value = Wire.read();
    value <<= 8;
    value |= Wire.read();
  }

  return value;
}
