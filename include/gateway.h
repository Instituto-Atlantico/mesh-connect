#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <lwip/raw.h>
#include "wifinode.h"

#define SCAN_ATTEMPTS 3

bool shouldEnableGateway(const char* gwSSID,
                         const char* gwPassword,
                         int scanAttempts = SCAN_ATTEMPTS);

class Gateway : public WifiNode {
 private:
  TaskHandle_t taskHandle;
  struct raw_pcb* icmpPcb;
  struct raw_pcb* tcpPcb;
  struct raw_pcb* udpPcb;

  void newPCB(struct raw_pcb** pcb, u8_t protocol);

 public:
  Gateway(const char* gwSSID,
          const char* gwPassword,
          DataQueue<message_t>* rxQueue,
          DataQueue<message_t>* txQueue);

  wifi_node_status_t getStatus() override;
  String getMode() override;

  void transmit() override;
  void announce();

  void receive(struct raw_pcb* pcb, struct pbuf* pbuff, const ip_addr_t* addr);
};

#endif
