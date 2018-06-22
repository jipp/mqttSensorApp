#ifndef sensor_h
#define sensor_h

#include <Arduino.h>
#include <Wire.h>

class Sensor {
public:
  byte sensorAddress = 0x00;
  byte sensorIDRegister = 0x00;
  byte sensorID = 0x00;
  bool isAvailable = false;
  virtual void begin() = 0;
  virtual void getValues() = 0;
  bool checkSensorAvailability(byte sensorAddress);
  bool checkSensorAvailability(byte sensorAddress, byte sensorIDRegister, byte value);
  void writeRegisterByte(byte sensorAddress, byte registerAddress);
  void writeRegisterByte(byte sensorAddress, byte registerAddress, byte registerValue);
  void writeRegisterInt(byte sensorAddress, uint16_t registerAddress);
  byte readRegisterByte(byte sensorAddress);
  byte readRegisterByte(byte sensorAddress, byte registerAddress);
  uint16_t readRegisterInt(byte sensorAddress);
  uint16_t readRegisterInt(byte sensorAddress, byte registerAddress);
};

#endif
