#include <Arduino.h>
#include <Sensor.h>
#include <Streaming.h>
#include <Wire.h>

#define BH1750_INSTRUCTION  CONTINUOUSLY_H_RESOLUTION_MODE

class BH1750 : public Sensor {
public:
  BH1750(int sensorAddress = 0x23);
  bool begin();
  uint16_t getIlluminance();

private:
  enum Instruction {
    POWER_DOWN = 0x00,
    POWER_ON = 0x01,
    RESET = 0x07,
    CONTINUOUSLY_H_RESOLUTION_MODE = 0x10,    // Measurement at 1lx resolution. Measurement time is approx 120ms.
    CONTINUOUSLY_H_RESOLUTION_MODE2 = 0x11,   // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
    CONTINUOUSLY_L_RESOLUTION_MODE = 0x13,    // Measurement at 4lx resolution. Measurement time is approx 16ms.
    ONE_TIME_H_RESOLUTION_MODE = 0x20,        // Measurement at 1lx resolution. Measurement time is approx 120ms.
    ONE_TIME_H_RESOLUTION_MODE2 = 0x21,       // Measurement at 0.5lx resolution. Measurement time is approx 120ms.
    ONE_TIME_L_RESOLUTION_MODE = 0x23         // Measurement at 4lx resolution. Measurement time is approx 16ms.
  };
  void wait(uint16 Instruction);
};
