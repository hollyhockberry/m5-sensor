// dhtmeter.cpp
// Copyright (c) 2021 Inaba

#include "dhtmeter.h"

DhtMeter::DhtMeter(uint8_t pin, uint8_t type)
: dht_(pin, type), fahrenheit_(false), temper_(0), humidity_(0), heatindex_(0) {
}

void DhtMeter::begin() {
  dht_.begin();
}

bool DhtMeter::update() {
  float humidity = dht_.readHumidity();
  float temper = dht_.readTemperature(Fahrenheit());
  if (isnan(humidity) || isnan(temper)) {
    return false;
  }
  temper_ = temper;
  humidity_ = humidity;
  heatindex_ = dht_.computeHeatIndex(temper, humidity, Fahrenheit());
  return true;
}
