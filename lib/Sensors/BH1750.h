#ifndef bh1750_h
#define bh1750_h

#include <Arduino.h>
#include <Sensor.h>
#include <Wire.h>

#define BH1750_ADDRESS  0x23
#define BH1750_MODE  CONTINUOUSLY_H_RESOLUTION_MODE

class BH1750 : public Sensor {
public:
  uint16_t illuminance;
  BH1750(byte sensorAddress = BH1750_ADDRESS);
  void begin();
  void getValues();

private:
  enum Mode {
    CONTINUOUSLY_H_RESOLUTION_MODE = 0x10,    // Measurement at 1lx resolution. Measurement time is approx 120ms.
    CONTINUOUSLY_H_RESOLUTION_MODE2 = 0x11,   // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
    CONTINUOUSLY_L_RESOLUTION_MODE = 0x13,    // Measurement at 4lx resolution. Measurement time is approx 16ms.
    ONE_TIME_H_RESOLUTION_MODE = 0x20,        // Measurement at 1lx resolution. Measurement time is approx 120ms.
    ONE_TIME_H_RESOLUTION_MODE2 = 0x21,       // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
    ONE_TIME_L_RESOLUTION_MODE = 0x23         // Measurement at 4lx resolution. Measurement time is approx 16ms.
  };
  void wait(uint16 Mode);
};

#endif
