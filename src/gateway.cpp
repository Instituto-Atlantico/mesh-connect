#include "gateway.h"
#include <WiFi.h>
#include <address.h>
#include <esp_wifi.h>
#include <packetprint.h>

#define MAX_CONNECT_RETRIES 10
#define CONNECT_DELAY_MILLS 1000    // 1 second
#define GW_ANNOUNCE_INTERVAL 10000  // 10 seconds

bool shouldEnableGateway(const char* gwSSID,
                         const char* gwPassword,
                         int scanAttempts) {
  for (int i = 0; i < scanAttempts; i++) {
    auto count = WiFi.scanNetworks(false, true, false);
    if (count < 0)
      throw "WiFi scan failed";

    for (int i = 0; i < count; i++) {
      auto foundSSID = WiFi.SSID(i);
      if (foundSSID.equals(gwSSID))
        return gwPassword != nullptr ||
               WiFi.encryptionType(i) == WIFI_AUTH_OPEN;
    }
  }

  return false;
}

static void gwTask(void* gwPointer) {
  auto gw = (Gateway*)gwPointer;
  auto lastAnnouce = -GW_ANNOUNCE_INTERVAL;

  for (;;) {
    gw->routeToInternet();

    auto now = millis();
    if (millis() - lastAnnouce >= GW_ANNOUNCE_INTERVAL) {
      gw->announce();
      lastAnnouce = now;
    }
  }
}

Gateway::Gateway(const char* gwSSID,
                 const char* gwPassword,
                 DataQueue<message_t>* rxQueue,
                 DataQueue<message_t>* txQueue) {
  this->rxQueue = rxQueue;
  this->txQueue = txQueue;

  int attempts = MAX_CONNECT_RETRIES;

  WiFi.begin(gwSSID, gwPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(CONNECT_DELAY_MILLS);
    if (attempts-- == 0)
      ESP.restart();
  }

  xTaskCreatePinnedToCore(gwTask, "GWTask", 10000, this, 0, &taskHandle,
                          WIFI_TASKS_CORE);

  WiFi.setAutoReconnect(true);
}

wifi_node_status_t Gateway::getStatus() {
  wifi_ap_record_t wifidata;
  auto result = esp_wifi_sta_get_ap_info(&wifidata);

  status.rssi = result == ESP_OK ? wifidata.rssi : 0;

  return WifiNode::getStatus();
}

void Gateway::announce() {
  auto localAddress = getLocalMACAddressAsUint32();
  auto message = newControlMessage(GW_ANNOUNCEMENT, localAddress);
  rxQueue->push(&message);
}

void Gateway::routeToInternet() {
  auto message = txQueue->poll();
  if (message == nullptr)
    return;
  auto l2Data = &message->data.layer2;
  auto ipv4 = (ipv4_headers_t*)l2Data->payload;
  void* payload = malloc(l2Data->length);
  memcpy(payload, l2Data->payload, l2Data->length);
  sendPacket(ipv4, payload, l2Data->length);

  free(l2Data->payload);
  free(message);
}
