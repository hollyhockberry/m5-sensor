// dhtmeter.h
// Copyright (c) 2021 Inaba

#ifndef DHTMETER_H_
#define DHTMETER_H_

#include <DHT.h>
#include "thermohygrometer.h"

class DhtMeter : public IThermoHygrometer {
 public:
  DhtMeter(uint8_t pin, uint8_t type);

  float Temperature() const override { return temper_; }
  float Humidity() const override { return humidity_; }
  float HeatIndex() const override { return heatindex_; }
  bool Fahrenheit() const override { return fahrenheit_; }
  void Fahrenheit(bool fahrenheit) { fahrenheit_ = fahrenheit; }

  void begin() override;
  bool update() override;
 private:
  DHT dht_;
  bool fahrenheit_;
  float temper_, humidity_, heatindex_;

  // DISALLOW_COPY_AND_ASSIGN(DhtMeter)
  DhtMeter(const DhtMeter&);
  void operator=(const DhtMeter&);
};

#endif  // DHTMETER_H_
