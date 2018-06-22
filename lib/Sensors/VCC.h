#ifndef vcc_h
#define vcc_h

#include <Arduino.h>
#include <Sensor.h>

class VCC : public Sensor {
public:
  float voltage;
  VCC();
  void begin();
  void getValues();
};

#endif
