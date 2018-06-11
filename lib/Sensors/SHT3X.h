#include <Arduino.h>

#include <Streaming.h>
#include <Wire.h>

class SHT3X {
private:
  int SHT3X_I2CADDR;
  enum Mode {
    SHT31_MEAS_HIGHREP_STRETCH = 0x2C06,
    SHT31_MEAS_MEDREP_STRETCH = 0x2C0D,
    SHT31_MEAS_LOWREP_STRETCH = 0x2C10,
    SHT31_MEAS_HIGHREP = 0x2400,
    SHT31_MEAS_MEDREP = 0x240B,
    SHT31_MEAS_LOWREP = 0x2416,
    SHT31_READSTATUS = 0xF32D,
    SHT31_CLEARSTATUS = 0x3041,
    SHT31_SOFTRESET = 0x30A2,
    SHT31_HEATEREN = 0x306D,
    SHT31_HEATERDIS = 0x3066,
};

public:
  SHT3X(byte addr = 0x44);
  void begin();
  bool isAvailable();
  void getValue();
  float temperature = 0.0;
  float humidity = 0.0;
};
