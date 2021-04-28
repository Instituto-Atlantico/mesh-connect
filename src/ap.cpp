#include "ap.h"
#include <layer2.h>
#include <layer3.h>
#include <metrics.h>

#define MAX_CLIENTS 4
#define MAX_TX_POWER 82
#define DEFAULT_CHANNEL 5

AccessPoint* apInstance = nullptr;

static const wifi_promiscuous_filter_t promiscuousFilter{
    .filter_mask = WIFI_PROMIS_FILTER_MASK_DATA_MPDU,
};

static void receiveCallback(void* buf, wifi_promiscuous_pkt_type_t type) {
  apInstance->receive((wifi_promiscuous_pkt_t*)buf);
}

AccessPoint::AccessPoint(const char* ssid,
                         DataQueue<message_t>* txQueue,
                         DataQueue<message_t>* rxQueue)
    : WifiNode(txQueue, rxQueue) {
  if (apInstance != nullptr)
    throw "Cannot have multiple AccessPoint instances";
  apInstance = this;

  if (!WiFi.softAP(ssid, nullptr, DEFAULT_CHANNEL, false, MAX_CLIENTS))
    throw "Cannot start AP";

  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_filter(&promiscuousFilter));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous_rx_cb(&receiveCallback));
  ESP_ERROR_CHECK(esp_wifi_set_promiscuous(true));
  ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE));
  ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(MAX_TX_POWER));
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
  if (l2data.length <= 0 || l2data.type != ETHER_TYPE_IPV4)
    return;

  auto ipv4 = (ipv4_headers_t*)l2data.payload;
  if (ipv4->destinationIP == 0 || ipv4->destinationIP == getIPAddress() ||
      isMulticast(ipv4->destinationIP)) {
    return;
  }

  if (ipv4->protocol != TCP && ipv4->protocol != UDP && ipv4->protocol != ICMP)
    return;

  // drop frame if it's larger than MTU
  if (l2data.length > WIFI_NODE_MTU) {
    sendFragmentationNeeded(ipv4);
    return;
  }

  // alocating memory for the data to be read elsewhere
  void* payload = malloc(l2data.length);
  if (payload == nullptr) {
    status.rxErrors++;
    return;
  }
  memcpy(payload, l2data.payload, l2data.length);

  // send to the queue
  auto message = newIPv4DataMessage(
      ipv4_datagram_t{.payload = payload, .size = (uint8_t)l2data.length});
  rxQueue->push(&message);
}

void AccessPoint::transmit() {
  auto message = txQueue->poll();
  if (message == nullptr || message->type != IPV4_DATAGRAM_MESSAGE) {
    free(message);
    return;
  }

  sendPacket((ipv4_headers_t*)message->data.ipv4Datagram.payload,
             message->data.ipv4Datagram.size, true);

  free(message->data.ipv4Datagram.payload);
  free(message);
}

String AccessPoint::getMode() {
  return "AP";
}
