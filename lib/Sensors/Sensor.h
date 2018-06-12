#include <Arduino.h>
#include <Streaming.h>

class Sensor {
public:
  bool isAvailable = false;

  Sensor(byte sensorAddress = 0x00, byte sensorID = 0x00);
  virtual bool begin() = 0;

private:
  byte sensorAddress;
  byte sensorID;
};
