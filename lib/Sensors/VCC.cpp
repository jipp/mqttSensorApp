#include <VCC.h>

VCC::VCC() {
}

void VCC::begin() {
  this->isAvailable = true;
}

void VCC::getValues() {
  this->voltage = ESP.getVcc() / 1000.0;
}
