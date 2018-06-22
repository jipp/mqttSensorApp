#ifndef bme280_h
#define bme280_h

#include <Arduino.h>
#include <Sensor.h>
#include <Wire.h>

#define BME280_ADDRESS  0x76
#define BME280_ID_REGISTER 0xD0
#define BME280_ID 0x60

class BME280 : public Sensor {
public:
  float temperature;
  float pressure;
  float humidity;
  BME280(byte sensorAddress = BME280_ADDRESS, byte sensorIDRegister = BME280_ID_REGISTER, byte sensorID = BME280_ID);
  void begin();
  void getValues();

private:
  enum CompensationParameterRegister {
    REGISTER_DIG_T1 = 0x88,
    REGISTER_DIG_T2 = 0x8A,
    REGISTER_DIG_T3 = 0x8C,
    REGISTER_DIG_P1 = 0x8E,
    REGISTER_DIG_P2 = 0x90,
    REGISTER_DIG_P3 = 0x92,
    REGISTER_DIG_P4 = 0x94,
    REGISTER_DIG_P5 = 0x96,
    REGISTER_DIG_P6 = 0x98,
    REGISTER_DIG_P7 = 0x9A,
    REGISTER_DIG_P8 = 0x9C,
    REGISTER_DIG_P9 = 0x9E,
    REGISTER_DIG_H1 = 0xA1,
    REGISTER_DIG_H2 = 0xE1,
    REGISTER_DIG_H3 = 0xE3,
    REGISTER_DIG_H4 = 0xE4,
    REGISTER_DIG_H5 = 0xE5,
    REGISTER_DIG_H6 = 0xE7
  };
  struct CompensationParameter {
    uint16_t t1;
    int16_t t2;
    int16_t t3;
    uint16_t p1;
    uint16_t p2;
    int16_t p3;
    int16_t p4;
    int16_t p5;
    int16_t p6;
    int16_t p7;
    int16_t p8;
    int16_t p9;
    uint8_t h1;
    int16_t h2;
    uint8_t h3;
    int16_t h4;
    int16_t h5;
    int8_t h6;
  } compensationParameter;
  void readCompensationData();
};

#endif
