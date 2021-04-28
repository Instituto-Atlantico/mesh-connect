#include "wifinode.h"
#include <esp_wifi.h>
#include <lwip/icmp.h>
#include <lwip/inet_chksum.h>
#include <lwip/pbuf.h>
#include <lwip/raw.h>
#include <lwip/sockets.h>

#define ICMP_DUR_FRAG_EXTRA_BYTES 8

static void wifiTask(void* pointer) {
  auto wifiNode = (WifiNode*)pointer;
  for (;;) {
    wifiNode->transmit();
  }
}

WifiNode::WifiNode(DataQueue<message_t>* txQueue,
                   DataQueue<message_t>* rxQueue) {
  memset(&status, 0, sizeof(status));
  this->txQueue = txQueue;
  this->rxQueue = rxQueue;

  xTaskCreatePinnedToCore(wifiTask, "WifiNode", 10000, this, 0, &wifiTaskHandle,
                          WIFI_TASKS_CORE);
}

IPAddress WifiNode::getIPAddress() {
  if (WiFi.getMode() == WIFI_MODE_APSTA)
    return WiFi.softAPIP();

  return WiFi.localIP();
}

uint8_t WifiNode::getNumberOfClients() {
  return WiFi.softAPgetStationNum();
}

wifi_node_status_t WifiNode::getStatus() {
  return this->status;
}

DataQueue<message_t>* WifiNode::getTXQueue() {
  return this->txQueue;
}

DataQueue<message_t>* WifiNode::getRXQueue() {
  return this->rxQueue;
}

void WifiNode::sendFragmentationNeeded(ipv4_headers_t* sourcePacket) {
  struct raw_pcb* pcb = raw_new(IP_PROTO_ICMP);

  size_t ipv4DataSize =
      getIPv4HeaderLength(sourcePacket) + ICMP_DUR_FRAG_EXTRA_BYTES;
  size_t size = sizeof(struct icmp_echo_hdr) + ipv4DataSize;

  struct pbuf* pbuff = pbuf_alloc(PBUF_IP, (uint16_t)size, PBUF_RAM);
  if (pbuff != nullptr && pbuff->len == pbuff->tot_len && pbuff->next == NULL) {
    struct icmp_echo_hdr* icmp = (struct icmp_echo_hdr*)pbuff->payload;

    ICMPH_TYPE_SET(icmp, ICMP_DUR);
    ICMPH_CODE_SET(icmp, ICMP_DUR_FRAG);
    icmp->chksum = 0;
    icmp->id = 0;
    icmp->seqno = htons(WIFI_NODE_MTU);
    memcpy(icmp + 1, sourcePacket, ipv4DataSize);
    icmp->chksum = inet_chksum(icmp, size);

    ip_addr addr = IPADDR4_INIT(sourcePacket->sourceIP);
    raw_sendto(pcb, pbuff, &addr);

    this->status.txFrames++;
  }

  pbuf_free(pbuff);
  raw_remove(pcb);
}

void WifiNode::sendPacket(ipv4_headers_t* headers,
                          size_t size,
                          bool keepLocalIP) {
  if (headers->protocol != IP_PROTO_ICMP && headers->protocol != IP_PROTO_UDP &&
      headers->protocol != IP_PROTO_TCP) {
    return;
  }

  auto headerSize = getIPv4HeaderLength(headers);
  auto payloadSize = size - headerSize;

  struct raw_pcb* pcb = raw_new(headers->protocol);
  struct pbuf* pbuff = pbuf_alloc(PBUF_IP, (u16_t)payloadSize, PBUF_RAM);

  if (pbuff != nullptr && pbuff->len == pbuff->tot_len && pbuff->next == NULL) {
    memcpy(pbuff->payload, ((uint8_t*)headers) + headerSize, payloadSize);

    ip_addr destinationIP = IPADDR4_INIT(headers->destinationIP);
    if (keepLocalIP)
      pcb->local_ip = IPADDR4_INIT(headers->sourceIP);

    raw_sendto(pcb, pbuff, &destinationIP);
  }

  pbuf_free(pbuff);
  raw_remove(pcb);
}
