#include <VCC.h>

VCC::VCC() {
  this->isAvailable = true;
}

void VCC::getValues() {
  this->voltage = ESP.getVcc() / 1000.0;
}
