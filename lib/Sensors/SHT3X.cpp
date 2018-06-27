 #include <SHT3X.h>

SHT3X::SHT3X(byte sensorAddress) {
  this->sensorAddress = sensorAddress;
}

void SHT3X::begin() {
  this->isAvailable = this->checkSensorAvailability(this->sensorAddress);
}

void SHT3X::getValues() {
  unsigned int data[6];

  writeRegister16(this->sensorAddress, SHT3X_MODE);
  wait(SHT3X_MODE);

  Wire.requestFrom((int) this->sensorAddress, 6);
  if (Wire.available() == 6) {
    for (int i=0; i<6; i++) {
  		data[i]=Wire.read();
  	};
  }

 	this->temperature = 175.0 * ((data[0] * 256.0) + data[1]) / 65535.0 - 45.0;
	this->humidity = 100.0 * ((data[3] * 256.0) + data[4]) / 65535.0;
}

void SHT3X::wait(uint32_t Mode) {
  switch(Mode) {
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
