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
  #define CHIP_ID_REGISTER  0xD0
  enum Mode {
    ULTRA_LOW_POWER = 0,        // 4.5ms
    STANDARD = 1,               // 7.5ms
    HIGH_RESOLUTION = 2,        // 13.5ms
    ULTRA_HIGH_RESOLUTION = 3   // 25.5ms
  };
  #define CONTROL_REGISTER_ADDRESS 0xF4
  enum Measurement {
    TEMPERATURE = 0x2E,         // 4.5ms
    PRESSURE_OSS0 = 0x34,       // 4.5ms
    PRESSURE_OSS1 = 0x74,       // 7.5ms
    PRESSURE_OSS2 = 0xB4,       // 13.5ms
    PRESSURE_OSS3 = 0xF4,       // 25.5ms
  };
};
