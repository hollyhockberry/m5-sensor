// m5-sensor
// Copyright (c) 2021 Inaba

#include <M5Atom.h>
#include "dhtmeter.h"

namespace {
const int kInterval = 5 * 1000;
DhtMeter dht(25, DHT11);
}  // namespace

void setup() {
  M5.begin(true, false, false);
  dht.begin();
  dht.Fahrenheit(false);
}

void loop() {
  int t = ::millis();

  if (dht.update()) {
    Serial.println(
      "Temperature: " + String(dht.Temperature()) +
      " Humidity: " + String(dht.Humidity()) +
      " HeatIndex: " + String(dht.HeatIndex()));
  }

  t = ::millis() - t;
  if (t < kInterval) {
    ::delay(kInterval - t);
  }
}
