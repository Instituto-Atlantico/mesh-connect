#ifndef _AP_CPP_
#define _AP_CPP_

#include <ap.h>
#include <layer2.h>
#include <layer3.h>

#define MAX_CLIENTS 4
#define MAX_TX_POWER 82
#define DEFAULT_CHANNEL 13

#define EXPONENTIAL_SMOOTHING_ALPHA 0.7

AccessPoint* apInstance = nullptr;

const wifi_promiscuous_filter_t promiscuousFilter{
    .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA_MPDU,
};

void receiveCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  apInstance->handleIncomingDataPacket((wifi_promiscuous_pkt_t*)buf);
}

AccessPoint::AccessPoint(const char* ssid) {
  if (apInstance != nullptr)
    throw "Cannot have multiple AccessPoint instances";
  apInstance = this;

  memset(&status, 0, sizeof(ap_status_t));

  if (!WiFi.softAP(ssid, nullptr, DEFAULT_CHANNEL, false, MAX_CLIENTS))
    throw "Cannot start AP";

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

void AccessPoint::handleIncomingDataPacket(wifi_promiscuous_pkt_t* packet) {
  status.rxFrames++;
  if (packet->rx_ctrl.rx_state != 0) {
    status.rxErrors++;
    return;
  }

  status.rssi = EXPONENTIAL_SMOOTHING_ALPHA * packet->rx_ctrl.rssi +
                (1 - EXPONENTIAL_SMOOTHING_ALPHA) * status.rssi;
  status.length = EXPONENTIAL_SMOOTHING_ALPHA * packet->rx_ctrl.sig_len +
                  (1 - EXPONENTIAL_SMOOTHING_ALPHA) * status.length;

  layer2_data_t l2data = getLayer2Data(packet->payload);

  switch (l2data.type) {
    case ETHER_TYPE_IPV4: {
      ipv4_headers_t* ipv4 = (ipv4_headers_t*)l2data.payload;
      Serial.print("SRC IPv4: ");
      Serial.print(ipv4->source[0]);
      Serial.print(".");
      Serial.print(ipv4->source[1]);
      Serial.print(".");
      Serial.print(ipv4->source[2]);
      Serial.print(".");
      Serial.print(ipv4->source[3]);
      Serial.print(" DST IPv4: ");
      Serial.print(ipv4->destination[0]);
      Serial.print(".");
      Serial.print(ipv4->destination[1]);
      Serial.print(".");
      Serial.print(ipv4->destination[2]);
      Serial.print(".");
      Serial.println(ipv4->destination[3]);
      break;
    }
    case ETHER_TYPE_ARP:
    case ETHER_TYPE_IPV6:
    case ETHER_TYPE_UNKNOWN:
      break;
  }
}

#endif
