#ifndef sensor_h
#define sensor_h

#include <Arduino.h>
#include <Streaming.h>
#include <Wire.h>

class Sensor {
public:
  bool isAvailable = false;
  int sensorAddress = 0x00;
  int sensorID = 0x00;

  virtual bool begin() = 0;
  bool checkAddress();
  void writeDataByte(byte data);
  void writeDataInt(uint16_t data);
  byte readDataByte();
  uint16_t readDataInt();
  byte readRegisterByte(byte registerAddress);
  uint16_t readRegisterInt(byte register);
};

#endif
