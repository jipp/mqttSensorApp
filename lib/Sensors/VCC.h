#include <Arduino.h>
#include <Sensor.h>

class VCC : public Sensor {
public:
  bool begin();
  int getVCC();

private:

};
