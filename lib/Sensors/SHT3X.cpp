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

  writeDataInt(SHT3X_CONDITION);

  wait(SHT3X_CONDITION);

  Wire.requestFrom(this->sensorAddress, 6);
  if (Wire.available() == 6) {
    for (int i=0; i<6; i++) {
  		data[i]=Wire.read();
  	};
  }

 	this->temperature = 175.0 * ((data[0] * 256.0) + data[1]) / 65535.0 - 45.0;
	this->humidity = 100.0 * ((data[3] * 256.0) + data[4]) / 65535.0;
}

void SHT3X::wait(uint32 Condition) {
  switch(Condition) {
    case H_REPEATABILITY_CLOCK_STRETCHING:
    delay(15);
    break;
    case M_REPEATABILITY_CLOCK_STRETCHING:
    delay(6);
    break;
    case L_REPEATABILITY_CLOCK_STRETCHING:
    delay(4);
    break;
    case H_REPEATABILITY:
    delay(15);
    break;
    case M_REPEATABILITY:
    delay(6);
    break;
    case L_REPEATABILITY:
    delay(4);
    break;
  };
}
