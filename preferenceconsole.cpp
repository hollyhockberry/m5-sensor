// preferenceconsole.cpp
// Copyright (c) 2021 Inaba

#include "preferenceconsole.h"
#include <InfluxDbClient.h>
#include <Preferences.h>
#include <FastLED.h>
#include "networkutil.h"

namespace {
const char NAME[] = "m5sensor";
}  // namespace

PreferenceConsole::PreferenceConsole()
: name_(), ssid_(), psk_(), influxhost_(), influxuser_(), influxpsk_() {
}

void PreferenceConsole::begin() {
  Preferences preferences;
  preferences.begin(NAME);
  name_ = preferences.getString("name");
  ssid_ = preferences.getString("ssid");
  psk_ = preferences.getString("psk");
  influxhost_ = preferences.getString("influxhost");
  influxuser_ = preferences.getString("influxuser");
  influxpsk_ = preferences.getString("influxpsk");
  influxport_ = preferences.getInt("influxport", 8086);
  dbname_ = preferences.getString("dbname");
  preferences.end();
}

namespace {

String readLine(bool echo, bool mask) {
  String line = "";
  while (true) {
    while (Serial.available()) {
      int c = Serial.read();
      if (c == '\n' || c == '\r') {
        if (echo) Serial.println();
        return line;
      }
      char ch = static_cast<char>(c);
      if (echo) Serial.print(mask ? '*' : ch);
      line += ch;
    }
    yield();
  }
}

String maskString(const String& str, bool mask) {
  if (!mask) return str;
  String s;
  for (int i = 0; i < str.length(); ++i) s += '*';
  return s;
}

String readKey(const char* key, const String& value, bool mask = false) {
  const bool cancelable = value[0] != '\0';
  while (true) {
    Serial.print(key);
    if (cancelable) Serial.print(String(" [") + maskString(value, mask) + "]");
    Serial.print(" : ");
    String line = readLine(true, mask);
    if (line[0] != '\0') return line;
    if (cancelable) return value;
  }
}

void setupServer(const PreferenceConsole& console,
                 Preferences* preferences, NetworkUtil* network) {
  Serial.println("Network settings:");
  while (true) {
    String name = readKey("Name", console.Name());
    String ssid = readKey("SSID", console.SSID());
    String psk = readKey("PSK", console.PSK(), true);

    Serial.print("conneting.. ");
    if (network->begin(name.c_str(), ssid.c_str(), psk.c_str(), 10000)) {
      preferences->putString("name", name);
      preferences->putString("ssid", ssid);
      preferences->putString("psk", psk);
      Serial.println("OK");
      Serial.println();
      return;
    }
    Serial.println("Failed");
    Serial.println();
  }
}

void setupInflux(const PreferenceConsole& console, Preferences* preferences) {
  InfluxDBClient client;
  Serial.println("InfluxDB settings:");
  while (true) {
    String host = readKey("Host", console.InfluxHost());
    int port = readKey("Port", String(console.InfluxPort())).toInt();
    String user = readKey("User", console.InfluxUser());
    String psk = readKey("PSK", console.InfluxPsk(), true);
    String db = readKey("Database", console.Database());

    String ip = NetworkUtil::resolveAddress(host.c_str());
    if (ip == "0.0.0.0") {
      Serial.println("Cannot resolve address.");
      continue;
    }
    String url = "http://" + ip + ":" + String(port, DEC);
    client.setConnectionParamsV1(url.c_str(), db.c_str(),
                                 user.c_str(), psk.c_str());
    Serial.print("conneting.. ");
    if (client.validateConnection()) {
      preferences->putString("influxhost", host);
      preferences->putString("influxuser", user);
      preferences->putString("influxpsk", psk);
      preferences->putInt("influxport", port);
      preferences->putString("dbname", db);
      Serial.println("OK");
      return;
    }
    Serial.println("Failed");
    Serial.println();
  }
}

}  // namespace

void PreferenceConsole::setup() {
  const int NUM_LEDS = 25;
  const int LED_PIN = 27;
  CRGB leds[NUM_LEDS];
  FastLED.addLeds<WS2812, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CRGB(0xff0000);
  }
  FastLED.show();

  Serial.println();
  Serial.println("Enter setup console");
  Serial.println();

  Preferences preferences;
  preferences.begin(NAME);
  NetworkUtil network;

  setupServer(*this, &preferences, &network);
  setupInflux(*this, &preferences);

  preferences.end();
  Serial.println("Setup done.");
  Serial.println("Reboot soon...");
  network.end();
  ::delay(3000);
  ESP.restart();
  while (true) {}
  // never reach
}
