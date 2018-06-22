#include <BME280.h>

BME280::BME280(byte sensorAddress, byte sensorIDRegister, byte sensorID) {
  this->sensorAddress = sensorAddress;
  this->sensorIDRegister = sensorIDRegister;
  this->sensorID = sensorID;
}

void BME280::begin() {
  this->isAvailable = this->checkSensorAvailability(this->sensorAddress, this->sensorIDRegister, this->sensorID);
  this->readCompensationData();
}

void BME280::getValues() {
  this->temperature = 0.0;
  this->pressure = 0.0;
  this->humidity = 0.0;
}

void BME280::readCompensationData() {
  this->compensationParameter.t1 = readRegisterInt(this->sensorAddress, REGISTER_DIG_T1);
  this->compensationParameter.t2 = readRegisterInt(this->sensorAddress, REGISTER_DIG_T2);
  this->compensationParameter.t3 = readRegisterInt(this->sensorAddress, REGISTER_DIG_T3);
  this->compensationParameter.p1 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P1);
  this->compensationParameter.p1 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P1);
  this->compensationParameter.p2 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P2);
  this->compensationParameter.p3 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P3);
  this->compensationParameter.p4 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P4);
  this->compensationParameter.p5 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P5);
  this->compensationParameter.p6 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P6);
  this->compensationParameter.p7 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P7);
  this->compensationParameter.p8 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P8);
  this->compensationParameter.p9 = readRegisterInt(this->sensorAddress, REGISTER_DIG_P9);
  this->compensationParameter.h1 = readRegisterByte(this->sensorAddress, REGISTER_DIG_H1);
  this->compensationParameter.h2 = readRegisterInt(this->sensorAddress, REGISTER_DIG_H2);
  this->compensationParameter.h3 = readRegisterByte(this->sensorAddress, REGISTER_DIG_H3);
  this->compensationParameter.h4 = readRegisterInt(this->sensorAddress, REGISTER_DIG_H4);
  this->compensationParameter.h5 = readRegisterInt(this->sensorAddress, REGISTER_DIG_H5);
  this->compensationParameter.h6 = readRegisterByte(this->sensorAddress, REGISTER_DIG_H6);
}
