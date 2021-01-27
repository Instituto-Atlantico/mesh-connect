#ifndef _GATEWAY_CPP_
#define _GATEWAY_CPP_

#include <WiFi.h>
#include <esp_wifi.h>
#include <gateway.h>

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
                 DataQueue<layer2_data_t>* rxQueue,
                 DataQueue<layer2_data_t>* txQueue) {
  this->rxQueue = rxQueue;
  this->txQueue = txQueue;

  WiFi.begin(gwSSID, nullptr);
  while (WiFi.status() != WL_CONNECTED)
    ;  // TODO add error handling

  // TODO enable auto connect
}

wifi_node_status_t Gateway::getStatus() {
  wifi_ap_record_t wifidata;
  auto result = esp_wifi_sta_get_ap_info(&wifidata);

  status.rssi = result == ESP_OK ? wifidata.rssi : 0;

  return WifiNode::getStatus();
}

#endif
