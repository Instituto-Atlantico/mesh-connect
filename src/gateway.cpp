#ifndef _GATEWAY_CPP_
#define _GATEWAY_CPP_

#include "gateway.h"
#include <WiFi.h>
#include <esp_wifi.h>

#define MAX_CONNECT_RETRIES 10
#define CONNECT_DELAY_MILLS 500

bool shouldEnableGateway(const char* gwSSID, int scanAttempts) {
  for (int i = 0; i < scanAttempts; i++) {
    auto count = WiFi.scanNetworks(false, true, false);
    if (count < 0)
      throw "WiFi scan failed";

    for (int i = 0; i < count; i++) {
      auto foundSSID = WiFi.SSID(i);
      if (foundSSID.equals(gwSSID)) {
        return WiFi.encryptionType(i) == WIFI_AUTH_OPEN;
      }
    }
  }

  return false;
}

Gateway::Gateway(const char* gwSSID,
                 DataQueue<message_t>* rxQueue,
                 DataQueue<message_t>* txQueue) {
  this->rxQueue = rxQueue;
  this->txQueue = txQueue;

  int attempts = MAX_CONNECT_RETRIES;

  WiFi.begin(gwSSID, nullptr);
  while (WiFi.status() != WL_CONNECTED) {
    delay(CONNECT_DELAY_MILLS);
    if (attempts-- == 0)
      ESP.restart();
  }

  WiFi.setAutoReconnect(true);
}

wifi_node_status_t Gateway::getStatus() {
  wifi_ap_record_t wifidata;
  auto result = esp_wifi_sta_get_ap_info(&wifidata);

  status.rssi = result == ESP_OK ? wifidata.rssi : 0;

  return WifiNode::getStatus();
}

#endif
