#include <SHT3X.h>

SHT3X::SHT3X(byte addr) {
  SHT3X_I2CADDR = addr;
}

void SHT3X::begin() {
}

bool SHT3X::isAvailable() {
  bool isAvailable = false;

  Wire.beginTransmission(SHT3X_I2CADDR);
  isAvailable = Wire.endTransmission();

  return isAvailable == 0 ? true : false;
}

void SHT3X::getValue() {
  unsigned int data[6];

  Wire.beginTransmission(SHT3X_I2CADDR);
  Wire.write(SHT31_MEAS_HIGHREP >> 8);
  Wire.write(SHT31_MEAS_HIGHREP & 0xFF);
  Wire.endTransmission();
  delay(500);

  Wire.requestFrom(SHT3X_I2CADDR, 6);
  if (Wire.available() == 6) {
    for (int i=0;i<6;i++) {
  		data[i]=Wire.read();
  	};
  }

 	temperature = ((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
	humidity = ((((data[3] * 256.0) + data[4]) * 100) / 65535.0);
}
