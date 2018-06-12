#include <VCC.h>

bool VCC::begin() {
  this->isAvailable = true;

  return this->isAvailable;
}

int VCC::getVCC() {
  return ESP.getVcc();
}
