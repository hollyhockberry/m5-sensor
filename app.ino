// m5-sensor
// Copyright (c) 2021 Inaba

#include <InfluxDbClient.h>
#include "dhtmeter.h"
#include "networkutil.h"
#include "preferenceconsole.h"

namespace {
const int kInterval = 60 * 1000;
bool ConfigMode = false;
PreferenceConsole preferences_;
NetworkUtil network;
InfluxDBClient influxclient;
Point dataPoint("environment");

DhtMeter dht(25, DHT11);
}  // namespace

void setup() {
  ::pinMode(39, INPUT_PULLUP);
  ConfigMode = ::digitalRead(39) == 0;

  preferences_.begin();

  if (ConfigMode) return;

  if (!network.begin(preferences_.Name().c_str(),
                     preferences_.SSID().c_str(),
                     preferences_.PSK().c_str(), 20 * 1000)) {
    ConfigMode = true;
    return;
  }

  String ip = NetworkUtil::resolveAddress(preferences_.InfluxHost().c_str());
  if (ip == "0.0.0.0") {
    ConfigMode = true;
    return;
  }
  String url = "http://" + ip + ":" + String(preferences_.InfluxPort(), DEC);
  influxclient.setConnectionParamsV1(
    url.c_str(),
    preferences_.Database().c_str(),
    preferences_.InfluxUser().c_str(),
    preferences_.InfluxPsk().c_str());

  if (!influxclient.validateConnection()) {
    ConfigMode = true;
    return;
  }
  dataPoint.addTag("place", preferences_.Name());

  dht.begin();
  dht.Fahrenheit(false);
}

void loop() {
  if (ConfigMode) {
    Serial.begin(115200);
    preferences_.setup();
  }

  int t = ::millis();

  if (network.update() && dht.update()) {
    dataPoint.clearFields();
    dataPoint.addField("temperature", dht.Temperature());
    dataPoint.addField("humidity", dht.Humidity());
    dataPoint.addField("heatindex", dht.HeatIndex());

    influxclient.writePoint(dataPoint);
  }

  t = ::millis() - t;
  if (t < kInterval) {
    ::delay(kInterval - t);
  }
}
