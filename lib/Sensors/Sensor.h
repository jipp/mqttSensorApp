#include <Arduino.h>
#include <Streaming.h>

class Sensor {
public:
  Sensor(byte sensorAddr);
  bool getIsAvailable();
  void getValues();
  float getVCC();
  float getTemperature();
  float getHumidity();
  float getPressure();
  float getIlluminance();

private:
  bool isAvailable = false;
  byte sensorAddr;
  float vcc = 0.0;
  float temperature = 0.0;
  float humidity = 0.0;
  float pressure = 0.0;
  float illuminance = 0.0;
};
