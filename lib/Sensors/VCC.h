#include <Arduino.h>
#include <Sensor.h>

class VCC : public Sensor {
public:
  float voltage;
  VCC();
  void getValues();
};
