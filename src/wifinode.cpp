#include "wifinode.h"
#include <esp_wifi.h>
#include <lwip/icmp.h>
#include <lwip/inet_chksum.h>
#include <lwip/raw.h>
#include <lwip/sockets.h>
#include <lwip/pbuf.h>

#define ICMP_DUR_FRAG_EXTRA_BYTES 8

WifiNode::WifiNode() {
  memset(&status, 0, sizeof(status));
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

void WifiNode::sendFragmentationNeeded(ipv4_headers_t* sourcePacket) {
  struct raw_pcb* pcb = raw_new(IP_PROTO_ICMP);

  size_t ipv4DataSize = 4 * sourcePacket->ihl + ICMP_DUR_FRAG_EXTRA_BYTES;
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

void sendPacket(ipv4_headers_t* headers, void* payload, size_t length){
  WiFiClient client;
  uint16_t port = 8888;
  char host[] = {"192.168.0.32"};


  if(!client.connect(host,port)){
    Serial.println("Fail in connection");
  }

  Serial.println("Connection OK");
  client.print("Hello World");
  client.stop();

}