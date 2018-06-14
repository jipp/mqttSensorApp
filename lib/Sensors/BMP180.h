#include <Arduino.h>
#include <Sensor.h>
#include <Streaming.h>
#include <Wire.h>

class BMP180 : public Sensor {
public:
  float temperature;
  float pressure;

  BMP180(int sensorAddress = 0x77, int sensorID = 0x55);
  bool begin();
  void getValues();
  void readCalibrationData();
  void readUncompensatedTemperature();
  void readUncompensatedPressure();
  float calculateTrueTemperature();
  float calculateTruePressure();

private:
  enum Mode {
    ULTRA_LOW_POWER = 0,        // 4.5ms
    STANDARD = 1,               // 7.5ms
    HIGH_RESOLUTION = 2,        // 13.5ms
    ULTRA_HIGH_RESOLUTION = 3   // 25.5ms
  };
  enum Measurement {
    TEMPERATURE = 0x2E,         // 4.5ms
    PRESSURE_OSS0 = 0x34,       // 4.5ms
    PRESSURE_OSS1 = 0x74,       // 7.5ms
    PRESSURE_OSS2 = 0xB4,       // 13.5ms
    PRESSURE_OSS3 = 0xF4,       // 25.5ms
  };
  enum CalibrationCoefficients{
    AC1 = 0xAA,
    AC2 = 0xAC,
    AC3 = 0xAE,
    AC4 = 0xB0,
    AC5 = 0xB2,
    AC6 = 0xB4,
    _B1 = 0xB6,
    B2 = 0xB8,
    MB = 0xBA,
    MC = 0xBC,
    MD = 0xBE
  };

  int16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
  uint16_t ac4, ac5, ac6;
  int16_t UT;
  uint32_t UP;
  int16_t B5;
};
