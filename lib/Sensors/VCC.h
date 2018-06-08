#include <Arduino.h>

class VCC {
private:

public:
  VCC();
  bool isAvailable();
  int getValue();
};
