#include <Arduino.h>

class VCC {
private:

public:
  VCC();
  void begin();
  bool isAvailable();
  int getValue();
};
