#include <Arduino.h>
#include <Streaming.h>
#include <Wire.h>

class BMP180 {
private:
  int BMP180_I2CADDR;
  enum Mode {
    BMP085_ULTRALOWPOWER = 0,
    BMP085_STANDARD = 1,
    BMP085_HIGHRES = 2,
    BMP085_ULTRAHIGHRES = 3
  };

public:
  BMP180(byte addr = 0x77);
  void begin();
  bool isAvailable();
  void getValue();
  float temperature = 0.0;
  float humidity = 0.0;
  float pressure = 0.0;
};
