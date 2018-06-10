#include <Arduino.h>

#include <Streaming.h>
#include <Wire.h>

class BH1750 {
private:
  int BH1750_I2CADDR;
  enum Mode {
    UNCONFIGURED = 0,
    CONTINUOUS_HIGH_RES_MODE = 0x10,    // Measurement at 1lx resolution. Measurement time is approx 120ms.
    CONTINUOUS_HIGH_RES_MODE_2 = 0x11,  // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
    CONTINUOUS_LOW_RES_MODE = 0x13,     // Measurement at 4lx resolution. Measurement time is approx 16ms.
    ONE_TIME_HIGH_RES_MODE = 0x20,      // Measurement at 1lx resolution. Measurement time is approx 120ms.
    ONE_TIME_HIGH_RES_MODE_2 = 0x21,    // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
    ONE_TIME_LOW_RES_MODE = 0x23        // Measurement at 1lx resolution. Measurement time is approx 120ms.
  };

public:
  BH1750(byte addr = 0x23);
  void begin();
  bool isAvailable();
  int getValue();
};
