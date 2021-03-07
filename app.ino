// m5-sensor
// Copyright (c) 2021 Inaba

#include <M5Atom.h>
#include <InfluxDbClient.h>
#include "dhtmeter.h"
#include "networkutil.h"

#include "settings.h"

namespace {
const int kInterval = 5 * 1000;
DhtMeter dht(25, DHT11);

NetworkUtil network;
InfluxDBClient influxclient;
Point dataPoint(INFLUXDB_MEASUREMENT);
}  // namespace

void setup() {
  M5.begin(true, false, false);

  network.begin("", SSID, PSK);

  String host = "http://" + NetworkUtil::resolveAddress(INFLUXDB_HOST)
                          + ":" + String(INFLUXDB_PORT, DEC);
  influxclient.setConnectionParamsV1(
    host.c_str(),
    INFLUXDB_DB_NAME,
    INFLUXDB_USER,
    INFLUXDB_PASSWORD);
  influxclient.validateConnection();

  dataPoint.addTag("place", INFLUXDB_TAG_PLACE);

  dht.begin();
  dht.Fahrenheit(false);
}

void loop() {
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
