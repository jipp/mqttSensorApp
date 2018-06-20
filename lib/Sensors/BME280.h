#include <Arduino.h>
#include <Sensor.h>
#include <Streaming.h>
#include <Wire.h>

class BME280 : public Sensor {
public:
  float temperature;
  float pressure;
  float humidity;
  BME280(int sensorAddress = 0x76, int sensorID = 0x60);
  bool begin();
  void getValues();

private:
};
