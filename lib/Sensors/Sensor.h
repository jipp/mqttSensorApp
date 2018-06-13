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
  void writeData(byte data);
  byte readDataByte();
  uint16_t readDataInt();

private:

};

#endif
