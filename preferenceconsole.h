// preferenceconsole.h
// Copyright (c) 2021 Inaba

#ifndef PREFERENCECONSOLE_H_
#define PREFERENCECONSOLE_H_

#include "WString.h"

class PreferenceConsole {
 public:
  const String& Name() const { return name_; }
  const String& SSID() const { return ssid_; }
  const String& PSK() const { return psk_; }
  const String& InfluxHost() const { return influxhost_; }
  int InfluxPort() const { return influxport_; }
  const String& InfluxUser() const { return influxuser_; }
  const String& InfluxPsk() const { return influxpsk_; }
  const String& Database() const { return dbname_; }

  void begin();
  void setup();

  PreferenceConsole();
 private:
  String name_, ssid_, psk_;
  String influxhost_, influxuser_, influxpsk_, dbname_;
  int influxport_;

  // DISALLOW_COPY_AND_ASSIGN(ServerPreferences)
  PreferenceConsole(const PreferenceConsole&);
  void operator=(const PreferenceConsole&);
};

#endif  // PREFERENCECONSOLE_H_
