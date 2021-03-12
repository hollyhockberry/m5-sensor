// m5-sensor
// Copyright (c) 2021 Inaba

#include <InfluxDbClient.h>
#include "dhtmeter.h"
#include "networkutil.h"
#include "preferenceconsole.h"

namespace {
const int kInterval = 60 * 1000;

void postData() {
  PreferenceConsole preferences;
  preferences.begin();

  NetworkUtil network;
  if (!network.begin(preferences.Name().c_str(),
                     preferences.SSID().c_str(),
                     preferences.PSK().c_str(), 20 * 1000)) {
    return;
  }

  String ip = NetworkUtil::resolveAddress(preferences.InfluxHost().c_str());
  if (ip == "0.0.0.0") {
    return;
  }
  String url = "http://" + ip + ":" + String(preferences.InfluxPort(), DEC);
  InfluxDBClient influxclient;
  influxclient.setConnectionParamsV1(
    url.c_str(),
    preferences.Database().c_str(),
    preferences.InfluxUser().c_str(),
    preferences.InfluxPsk().c_str());

  if (!influxclient.validateConnection()) {
    return;
  }
  Point dataPoint("environment");
  dataPoint.addTag("place", preferences.Name());

  DhtMeter dht(25, DHT11);
  dht.begin();
  dht.Fahrenheit(false);

  if (dht.update()) {
    dataPoint.clearFields();
    dataPoint.addField("temperature", dht.Temperature());
    dataPoint.addField("humidity", dht.Humidity());
    dataPoint.addField("heatindex", dht.HeatIndex());

    influxclient.writePoint(dataPoint);
  }
  network.end();
}

}  // namespace

void setup() {
  ::pinMode(39, INPUT_PULLUP);

  if (::digitalRead(39) != 0) {
    int t = ::millis();
    postData();
    // enter sleep
    t = ::millis() - t;
    ESP.deepSleep((kInterval - t) * 1000);
    for (;;) {}
    // never reach
  }
}

void loop() {
  Serial.begin(115200);
  PreferenceConsole preferences;
  preferences.begin();
  preferences.setup();
  // never readch
}
