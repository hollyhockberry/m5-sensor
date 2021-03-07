// networkutil.cpp
// Copyright (c) 2021 Inaba

#include "networkutil.h"
#include <ESPmDNS.h>

NetworkUtil::NetworkUtil() : wifiMulti_() {
}

bool NetworkUtil::begin(const char hostname[],
                        const char ssid[], const char psk[], int timeout) {
  int t = ::millis();
  WiFi.mode(WIFI_STA);
  wifiMulti_.addAP(ssid, psk);
  while (wifiMulti_.run() != WL_CONNECTED) {
    if (time > 0 && ((::millis() - t) > timeout)) return false;
    ::delay(500);
  }
  if (!MDNS.begin(hostname)) {
    return false;
  }
  return true;
}

void NetworkUtil::end() {
  WiFi.disconnect();
}

bool NetworkUtil::update() {
  return wifiMulti_.run() == WL_CONNECTED;
}

String NetworkUtil::resolveAddress(const char mdnsname[], uint32_t timeout) {
  IPAddress ip = MDNS.queryHost(mdnsname, timeout);
  return ip.toString();
}
