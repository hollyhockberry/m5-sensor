// networkutil.h
// Copyright (c) 2021 Inaba

#ifndef NETWORKUTIL_H_
#define NETWORKUTIL_H_

#include <WiFiMulti.h>

class NetworkUtil {
 public:
  NetworkUtil();

  bool begin(const char hostname[], const char ssid[], const char psk[],
             int timeout = 0);
  bool update();

  static String resolveAddress(const char mdnsname[], uint32_t timeout = 2000U);
 private:
  WiFiMulti wifiMulti_;

  NetworkUtil(const NetworkUtil&);
  void operator=(const NetworkUtil&);
};

#endif  // NETWORKUTIL_H_
