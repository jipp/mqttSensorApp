#include <BMP180.h>

BMP180::BMP180(byte addr) {
  BMP180_I2CADDR = addr;
}

void BMP180::begin() {
}

bool BMP180::isAvailable() {
  bool isAvailable = false;

  Wire.beginTransmission(BMP180_I2CADDR);
  isAvailable = Wire.endTransmission();

  return isAvailable == 0 ? true : false;
}

void BMP180::getValue() {
  temperature = 0.0;
  humidity = 0.0;
  pressure = 0.0;
}
