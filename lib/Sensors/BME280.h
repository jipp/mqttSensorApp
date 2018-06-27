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
  int32_t t_fine;
  enum CompensationParameterRegister {
    REGISTER_DIG_T1_LSB = 0x88,
    REGISTER_DIG_T1_MSB = 0x89,
    REGISTER_DIG_T2_LSB = 0x8A,
    REGISTER_DIG_T2_MSB = 0x8B,
    REGISTER_DIG_T3_LSB = 0x8C,
    REGISTER_DIG_T3_MSB = 0x8D,
    REGISTER_DIG_P1_LSB = 0x8E,
    REGISTER_DIG_P1_MSB = 0x8F,
    REGISTER_DIG_P2_LSB = 0x90,
    REGISTER_DIG_P2_MSB = 0x91,
    REGISTER_DIG_P3_LSB = 0x92,
    REGISTER_DIG_P3_MSB = 0x93,
    REGISTER_DIG_P4_LSB = 0x94,
    REGISTER_DIG_P4_MSB = 0x95,
    REGISTER_DIG_P5_LSB = 0x96,
    REGISTER_DIG_P5_MSB = 0x97,
    REGISTER_DIG_P6_LSB = 0x98,
    REGISTER_DIG_P6_MSB = 0x99,
    REGISTER_DIG_P7_LSB = 0x9A,
    REGISTER_DIG_P7_MSB = 0x9B,
    REGISTER_DIG_P8_LSB = 0x9C,
    REGISTER_DIG_P8_MSB = 0x9D,
    REGISTER_DIG_P9_LSB = 0x9E,
    REGISTER_DIG_P9_MSB = 0x9F,
    REGISTER_DIG_H1 = 0xA1,
    REGISTER_DIG_H2_LSB = 0xE1,
    REGISTER_DIG_H2_MSB = 0xE2,
    REGISTER_DIG_H3 = 0xE3,
    REGISTER_DIG_H4_MSB = 0xE4,
    REGISTER_DIG_H4_LSB = 0xE5,
    REGISTER_DIG_H5_LSB = 0xE5,
    REGISTER_DIG_H5_MSB = 0xE6,
    REGISTER_DIG_H6 = 0xE7
  };
  enum BME280Register {
    CTRL_HUM = 0xF2,
    CTRL_MEAS = 0xF4,
    CONFIG = 0xF5,
    PRESS_MSB = 0xF7,
    PRESS_LSB = 0xF8,
    PRESS_XLSB = 0xF9,
    TEMP_MSB = 0xFA,
    TEMP_LSB = 0xFB,
    TEMP_XLSB = 0xFC,
    HUM_MSB = 0xFD,
    HUM_LSB = 0xFE
  };
  struct CompensationParameter {
    uint16_t t1;
    int16_t t2;
    int16_t t3;
    uint16_t p1;
    int16_t p2;
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
  enum SensorMode {
    MODE_SLEEP = 0b00,
    MODE_FORCED = 0b01,
    MODE_NORMAL = 0b11
  };
  enum OversamplingSetting {
    SAMPLING_OFF = 0b000,
    SAMPLING_1 = 0b001,
    SAMPLING_2 = 0b010,
    SAMPLING_4 = 0b011,
    SAMPLING_8 = 0b100,
    SAMPLING_16 = 0b101
  };
  enum FilterSetting {
    FILTER_OFF = 0b000,
    FILTER_2 = 0b001,
    FILTER_4 = 0b010,
    FILTER_8 = 0b011,
    FILTER_16 = 0b100
  };
  enum StandbySetting {
    STANDBY_0_5 = 0b000,
    STANDBY_62_5 = 0b001,
    STANDBY_125 = 0b010,
    STANDBY_250 = 0b011,
    STANDBY_500 = 0b100,
    STANDBY_1000 = 0b101,
    STANDBY_10 = 0b110,
    STANDBY_20 = 0b111
  };
  struct BME289Settings {
    uint8_t sensorMode = MODE_NORMAL;
  	uint8_t standbySetting = STANDBY_0_5;
  	uint8_t filterSetting = FILTER_OFF;
  	uint8_t temperatureOversamplingSetting = SAMPLING_1;
  	uint8_t pressureOversamplingSetting = SAMPLING_1;
  	uint8_t hummidityOversamplingSetting = SAMPLING_1;
  } bme280Settings;
  void readCompensationData();
  void setStandby(uint8_t t_sb);
  void setFilter(uint8_t filter);
  void setTemperatureOversampling(uint8_t osrs_t);
  void setPressureOversampling(uint8_t osrs_p);
  void setHumidityOversampling(uint8_t osrs_h);
  void setMode(uint8_t mode);
  float getHumidity(void);
  float getTemperature(void);
  float getPressure(void);
};

#endif
