#include "VCC.h"

VCC::VCC() {
}

void VCC::begin() {
}

bool VCC::isAvailable() {
  return true;
}

int VCC::getValue() {
  return ESP.getVcc();
}
