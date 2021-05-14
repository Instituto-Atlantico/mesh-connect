#include "gateway.h"
#include <WiFi.h>
#include <address.h>
#include <esp_wifi.h>

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

static void announceTask(void* gwPointer) {
  auto gw = (Gateway*)gwPointer;
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(GW_ANNOUNCE_INTERVAL));
    gw->announce();
  }
}

static u8_t readIncomingPacket(void* arg,
                               struct raw_pcb* pcb,
                               struct pbuf* pbuff,
                               const ip_addr_t* addr) {
  Gateway* gateway = (Gateway*)arg;
  gateway->receive(pcb, pbuff, addr);
  pbuf_free(pbuff);
  return 1;
}

Gateway::Gateway(const char* gwSSID,
                 const char* gwPassword,
                 DataQueue<message_t>* txQueue,
                 DataQueue<message_t>* rxQueue)
    : WifiNode(txQueue, rxQueue) {
  int attempts = MAX_CONNECT_RETRIES;

  WiFi.begin(gwSSID, gwPassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(CONNECT_DELAY_MILLS);
    if (attempts-- == 0)
      ESP.restart();
  }

  xTaskCreatePinnedToCore(announceTask, "GWAnnounceTask", 10000, this, 0,
                          &taskHandle, WIFI_TASKS_CORE);

  WiFi.setAutoReconnect(true);

  newPCB(&icmpPcb, ICMP);
  newPCB(&tcpPcb, TCP);
  newPCB(&udpPcb, UDP);
}

void Gateway::newPCB(struct raw_pcb** pcb, u8_t protocol) {
  auto newPcb = raw_new(protocol);
  if (newPcb == nullptr) {
    Serial.println("Could not allocate PCB\n");
    ESP.restart();
  }

  *pcb = newPcb;

  auto err = raw_bind(newPcb, IP4_ADDR_ANY);
  if (err != ERR_OK) {
    Serial.println("Could bind PCB to all IPv4 addresses\n");
    ESP.restart();
  }

  raw_recv(newPcb, readIncomingPacket, this);
}

wifi_node_status_t Gateway::getStatus() {
  wifi_ap_record_t wifidata;
  auto result = esp_wifi_sta_get_ap_info(&wifidata);

  status.rssi = result == ESP_OK ? wifidata.rssi : 0;

  return WifiNode::getStatus();
}

void Gateway::announce() {
  auto message = newControlMessage(GW_ANNOUNCEMENT);
  rxQueue->push(&message);
}

void Gateway::transmit() {
  auto message = txQueue->poll();
  if (message == nullptr || message->type != IPV4_DATAGRAM_MESSAGE) {
    free(message);
    return;
  }

  sendPacket((ipv4_headers_t*)message->data.ipv4Datagram.payload,
             message->data.ipv4Datagram.size, false);

  free(message->data.ipv4Datagram.payload);
  free(message);
}

void Gateway::receive(struct raw_pcb* pcb,
                      struct pbuf* pbuff,
                      const ip_addr_t* addr) {
  auto ipv4 = (ipv4_headers_t*)pbuff->payload;

  if (ipv4->destinationIP != getIPAddress() ||
      (ipv4->protocol != TCP && ipv4->protocol != UDP &&
       ipv4->protocol != ICMP)) {
    return;
  }

  if (pbuff->len > WIFI_NODE_MTU && getDontFragmentBit(ipv4)) {
    sendFragmentationNeeded(ipv4);
    return;
  }

  void* payload = malloc(pbuff->len);
  memcpy(payload, pbuff->payload, pbuff->len);

  auto message = newIPv4DataMessage(
      ipv4_datagram_t{.payload = payload, .size = (uint8_t)pbuff->len});
  rxQueue->push(&message);
}

String Gateway::getMode() {
  return "GW";
}
