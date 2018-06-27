#include <BME280.h>

BME280::BME280(byte sensorAddress, byte sensorIDRegister, byte sensorID) {
  this->sensorAddress = sensorAddress;
  this->sensorIDRegister = sensorIDRegister;
  this->sensorID = sensorID;
}

void BME280::begin() {
  this->isAvailable = this->checkSensorAvailability(this->sensorAddress, this->sensorIDRegister, this->sensorID);
  if (this->isAvailable) {
    this->readCompensationData();
    this->setStandby(bme280Settings.standbySetting);
    this->setFilter(bme280Settings.filterSetting);
    this->setTemperatureOversampling(bme280Settings.temperatureOversamplingSetting);
    this->setPressureOversampling(bme280Settings.pressureOversamplingSetting);
    this->setHumidityOversampling(bme280Settings.hummidityOversamplingSetting);
    this->setMode(bme280Settings.sensorMode);
  }
}

void BME280::getValues() {
  this->temperature = this->getTemperature();
  this->pressure = this->getPressure();
  this->humidity = this->getHumidity();
}

void BME280::readCompensationData() {
  this->compensationParameter.t1 = (readRegister8(this->sensorAddress, REGISTER_DIG_T1_MSB) <<  8) + readRegister8(this->sensorAddress, REGISTER_DIG_T1_LSB);
  this->compensationParameter.t2 = (readRegister8(this->sensorAddress, REGISTER_DIG_T2_MSB) <<  8) + readRegister8(this->sensorAddress, REGISTER_DIG_T2_LSB);
  this->compensationParameter.t3 = (readRegister8(this->sensorAddress, REGISTER_DIG_T3_MSB) <<  8) + readRegister8(this->sensorAddress, REGISTER_DIG_T3_LSB);

  this->compensationParameter.p1 = (readRegister8(this->sensorAddress, REGISTER_DIG_P1_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P1_LSB);
  this->compensationParameter.p2 = (readRegister8(this->sensorAddress, REGISTER_DIG_P2_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P2_LSB);
  this->compensationParameter.p3 = (readRegister8(this->sensorAddress, REGISTER_DIG_P3_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P3_LSB);
  this->compensationParameter.p4 = (readRegister8(this->sensorAddress, REGISTER_DIG_P4_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P4_LSB);
  this->compensationParameter.p5 = (readRegister8(this->sensorAddress, REGISTER_DIG_P5_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P5_LSB);
  this->compensationParameter.p6 = (readRegister8(this->sensorAddress, REGISTER_DIG_P6_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P6_LSB);
  this->compensationParameter.p7 = (readRegister8(this->sensorAddress, REGISTER_DIG_P7_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P7_LSB);
  this->compensationParameter.p8 = (readRegister8(this->sensorAddress, REGISTER_DIG_P8_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P8_LSB);
  this->compensationParameter.p9 = (readRegister8(this->sensorAddress, REGISTER_DIG_P9_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_P9_LSB);

  this->compensationParameter.h1 = readRegister8(this->sensorAddress, REGISTER_DIG_H1);
  this->compensationParameter.h2 = (readRegister8(this->sensorAddress, REGISTER_DIG_H2_MSB) << 8) + readRegister8(this->sensorAddress, REGISTER_DIG_H2_LSB);
  this->compensationParameter.h3 = readRegister8(this->sensorAddress, REGISTER_DIG_H3);
  this->compensationParameter.h4 = (readRegister8(this->sensorAddress, REGISTER_DIG_H4_MSB) << 4) + (readRegister8(this->sensorAddress, REGISTER_DIG_H4_LSB) & 0x0F);
  this->compensationParameter.h5 = (readRegister8(this->sensorAddress, REGISTER_DIG_H5_MSB) << 4) + ((readRegister8(this->sensorAddress, REGISTER_DIG_H5_LSB) >> 4) & 0x0F);
  this->compensationParameter.h6 = readRegister8(this->sensorAddress, REGISTER_DIG_H6);
}

void BME280::setStandby(uint8_t t_sb) {
  uint8_t data;

  data = readRegister8(this->sensorAddress, CONFIG);
  data &= ~( (1<<7) | (1<<6) | (1<<5) );
  data |= (t_sb << 5);
  writeRegister8(this->sensorAddress, CONFIG, data);
}

void BME280::setFilter(uint8_t filter) {
  uint8_t data;

  data = readRegister8(this->sensorAddress, CONFIG);
  data &= ~( (1<<4) | (1<<3) | (1<<2) );
  data |= (filter << 2);
  writeRegister8(this->sensorAddress, CONFIG, data);
}

void BME280::setTemperatureOversampling(uint8_t osrs_t) {
  uint8_t data;

  data = readRegister8(this->sensorAddress, CTRL_MEAS);
  data &= ~( (1<<7) | (1<<6) | (1<<5) );
  data |= osrs_t << 5;
  writeRegister8(this->sensorAddress, CTRL_MEAS , data);
}

void BME280::setPressureOversampling(uint8_t osrs_p) {
  uint8_t data;

  data = readRegister8(this->sensorAddress, CTRL_MEAS);
  data &= ~( (1<<4) | (1<<3) | (1<<2) );
  data |= osrs_p << 2;
  writeRegister8(this->sensorAddress, CTRL_MEAS , data);
}

void BME280::setHumidityOversampling(uint8_t osrs_h) {
  uint8_t data;

  data = readRegister8(this->sensorAddress, CTRL_HUM);
  data &= ~( (1<<2) | (1<<1) | (1<<0) );
  data |= osrs_h << 0;
  writeRegister8(this->sensorAddress, CTRL_HUM, data);
}

void BME280::setMode(uint8_t mode) {
  uint8_t data;

  data = readRegister8(this->sensorAddress, CTRL_MEAS);
  data &= ~( (1<<1) | (1<<0) );
  data |= mode;
  writeRegister8(this->sensorAddress, CTRL_MEAS, data);
}

float BME280::getHumidity() {
  uint8_t hum_msb;
  uint8_t hum_lsb;
  int32_t adc_H;
  uint32_t var1;

  hum_msb = readRegister8(this->sensorAddress, HUM_MSB);
  hum_lsb = readRegister8(this->sensorAddress, HUM_LSB);

  adc_H = (hum_msb << 8) + (hum_lsb);

  var1 = (t_fine - ((int32_t)76800));
  var1 = (((((adc_H << 14) - (((int32_t)compensationParameter.h4) << 20) - (((int32_t)compensationParameter.h5) * var1)) + ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)compensationParameter.h6)) >> 10) * (((var1 * ((int32_t)compensationParameter.h3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)compensationParameter.h2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)compensationParameter.h1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);

  return (var1>>12) / 1024.0;
}

float BME280::getTemperature() {
  uint8_t temp_msb;
  uint8_t temp_lsb;
  uint8_t temp_xlsb;
  int32_t adc_T;
  int32_t var1;
  int32_t var2;
  int32_t T;

  temp_msb = readRegister8(this->sensorAddress, TEMP_MSB);
  temp_lsb = readRegister8(this->sensorAddress, TEMP_LSB);
  temp_xlsb = readRegister8(this->sensorAddress, TEMP_XLSB);

  adc_T = ((uint32_t)temp_msb << 12) | ((uint32_t)temp_lsb << 4) | ((temp_xlsb >> 4) & 0x0F);

  var1 = ((((adc_T>>3) - ((int32_t)compensationParameter.t1<<1))) * ((int32_t)compensationParameter.t2)) >> 11;
  var2 = (((((adc_T>>4) - ((int32_t)compensationParameter.t1)) * ((adc_T>>4) - ((int32_t)compensationParameter.t1))) >> 12) * ((int32_t)compensationParameter.t3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;

  return T / 100.0;
}

float BME280::getPressure() {
  uint8_t press_msb;
  uint8_t press_lsb;
  uint8_t press_xlsb;
  int32_t adc_P;
  int64_t var1;
  int64_t var2;
  int64_t P;

  press_msb = readRegister8(this->sensorAddress, PRESS_MSB);
  press_lsb = readRegister8(this->sensorAddress, PRESS_LSB);
  press_xlsb = readRegister8(this->sensorAddress, PRESS_XLSB);

  adc_P = ((uint32_t)press_msb << 12) | ((uint32_t)press_lsb << 4) | ((press_xlsb >> 4) & 0x0F);

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)compensationParameter.p6;
  var2 = var2 + ((var1 * (int64_t)compensationParameter.p5)<<17);
  var2 = var2 + (((int64_t)compensationParameter.p4)<<35);
  var1 = ((var1 * var1 * (int64_t)compensationParameter.p3)>>8) + ((var1 * (int64_t)compensationParameter.p2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)compensationParameter.p1)>>33;
  if (var1 == 0)
  {
    return 0;
  }
  P = 1048576 - adc_P;
  P = (((P<<31) - var2)*3125)/var1;
  var1 = (((int64_t)compensationParameter.p9) * (P>>13) * (P>>13)) >> 25;
  var2 = (((int64_t)compensationParameter.p8) * P) >> 19;
  P = ((P + var1 + var2) >> 8) + (((int64_t)compensationParameter.p7)<<4);

  return P / 256.0 / 100.0;
}
