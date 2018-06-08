#include "VCC.h"

VCC::VCC() {

}

bool VCC::isAvailable() {
  return true;
}

int VCC::getValue() {
  return ESP.getVcc();
}
