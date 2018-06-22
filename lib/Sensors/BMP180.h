#ifndef bmp180_h
#define bmp180_h

#include <Arduino.h>
#include <Sensor.h>
#include <Wire.h>

#define BMP180_ADDRESS  0x77
#define BMP180_ID_REGISTER 0xD0
#define BMP180_ID 0x55
#define BMP180_MODE  ULTRA_HIGH_RESOLUTION

class BMP180 : public Sensor {
public:
  float temperature;
  float pressure;
  BMP180(byte sensorAddress = BMP180_ADDRESS, byte sensorRegister = BMP180_ID_REGISTER, byte sensorID = BMP180_ID);
  void begin();
  void getValues();

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
  enum CalibrationCoefficientsRegister {
    REGISTER_AC1 = 0xAA,
    REGISTER_AC2 = 0xAC,
    REGISTER_AC3 = 0xAE,
    REGISTER_AC4 = 0xB0,
    REGISTER_AC5 = 0xB2,
    REGISTER_AC6 = 0xB4,
    REGISTER_B1 = 0xB6,
    REGISTER_B2 = 0xB8,
    REGISTER_MB = 0xBA,
    REGISTER_MC = 0xBC,
    REGISTER_MD = 0xBE
  };
  struct CalibrationCoefficients {
    int16_t ac1;
    int16_t ac2;
    int16_t ac3;
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
    int16_t mb;
    int16_t mc;
    int16_t md;
  } calibrationCoefficients;
  int32_t UT;
  int32_t UP;
  int32_t B5;
  void wait(uint16_t Mode);
  void readCalibrationData();
  void readUncompensatedTemperature();
  void readUncompensatedPressure();
  float calculateTrueTemperature();
  float calculateTruePressure();
};

#endif
