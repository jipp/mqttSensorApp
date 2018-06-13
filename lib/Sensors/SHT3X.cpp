#include <SHT3X.h>

SHT3X::SHT3X(int sensorAddress) {
  this->sensorAddress = sensorAddress;
}

bool SHT3X::begin() {
  this->isAvailable = checkAddress();

  return this->isAvailable;
}

void SHT3X::getValues() {
  unsigned int data[6];

  writeDataInt(H_REPEATABILITY_CLOCK_STRETCHING);
  delay(15);

  Wire.requestFrom(this->sensorAddress, 6);
  if (Wire.available() == 6) {
    for (int i=0;i<6;i++) {
  		data[i]=Wire.read();
  	};
  }

 	this->temperature = 175.0 * ((data[0] * 256.0) + data[1]) / 65535.0 - 45.0;
	this->humidity = 100.0 * ((data[3] * 256.0) + data[4]) / 65535.0;
}
