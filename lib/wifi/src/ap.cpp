#ifndef _AP_CPP_
#define _AP_CPP_

#include <ap.h>
#include <layer2.h>
#include <layer3.h>
#include <metrics.h>

#define MAX_CLIENTS 4
#define MAX_TX_POWER 82
#define DEFAULT_CHANNEL 13

AccessPoint* apInstance = nullptr;

static const wifi_promiscuous_filter_t promiscuousFilter{
    .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA_MPDU,
};

static void receiveCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  apInstance->receive((wifi_promiscuous_pkt_t*)buf);
}

static void transmitTask(void* pointer) {
  auto ap = (AccessPoint*)pointer;
  for (;;) {
    ap->transmit();
  }
}

AccessPoint::AccessPoint(const char* ssid,
                         DataQueue* rxQueue,
                         DataQueue* txQueue) {
  if (apInstance != nullptr)
    throw "Cannot have multiple AccessPoint instances";
  apInstance = this;

  memset(&status, 0, sizeof(ap_status_t));
  this->rxQueue = rxQueue;
  this->txQueue = txQueue;

  if (!WiFi.softAP(ssid, nullptr, DEFAULT_CHANNEL, false, MAX_CLIENTS))
    throw "Cannot start AP";

  xTaskCreatePinnedToCore(transmitTask, "WiFiTransmitter", 10000, this, 0,
                          &taskHandle, 1);

  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&promiscuousFilter));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&receiveCallback));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(MAX_TX_POWER));
}

IPAddress AccessPoint::getIPAddress() {
  return WiFi.softAPIP();
}

uint8_t AccessPoint::getNumberOfClients() {
  return WiFi.softAPgetStationNum();
}

ap_status_t AccessPoint::getStatus() {
  return this->status;
}

void AccessPoint::receive(wifi_promiscuous_pkt_t* packet) {
  // update counters
  status.rxFrames++;
  if (packet->rx_ctrl.rx_state != 0) {
    status.rxErrors++;
    return;
  }

  exponentialSmoothing<int16_t>(status.rssi, packet->rx_ctrl.rssi);
  exponentialSmoothing<uint16_t>(status.length, packet->rx_ctrl.sig_len);

  // get layer 2 data
  auto l2data = getLayer2Data(packet->payload, packet->rx_ctrl.sig_len);
  if (l2data.length <= 0)
    return;

  // alocating memory for the data to be read elsewhere
  void* payload = malloc(l2data.length);
  if (payload == nullptr) {
    status.rxErrors++;
    return;
  }
  memcpy(payload, l2data.payload, l2data.length);
  l2data.payload = payload;

  // send to the queue
  rxQueue->push(&l2data);
}

void AccessPoint::transmit() {
  auto layer2Data = txQueue->poll();
  if (layer2Data == nullptr)
    return;

  // send over 802.11

  free(layer2Data->payload);
  free(layer2Data);
}

#endif
