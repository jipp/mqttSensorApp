#include <Arduino.h>
#include <Sensor.h>
#include <Streaming.h>
#include <Wire.h>

class SHT3X : public Sensor {
public:
  float temperature;
  float humidity;
  
  SHT3X(int sensorAddress = 0x44);
  bool begin();
  void getValues();


private:
  enum Mode {
    H_REPEATABILITY_CLOCK_STRETCHING = 0x2C06,  // 15ms
    M_REPEATABILITY_CLOCK_STRETCHING = 0x2C0D,  // 6ms
    L_REPEATABILITY_CLOCK_STRETCHING = 0x2C10,  // 6ms
    H_REPEATABILITY = 0x2400,  // 15ms
    M_REPEATABILITY = 0x240B,  // 6ms
    L_REPEATABILITY = 0x2416,  // 6ms
    READ_OUT_OF_STATUS_REGISTER = 0xF32D,
    CLEAR_STATUS = 0x3041,
    BREAK = 0x3093,
    SOFT_RESET = 0x30A2,
    HEATER_ENABLE = 0x306D,
    HEATRER_DISABLED = 0x3066,
  };
};
