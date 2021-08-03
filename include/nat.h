#ifndef _NAT_H_
#define _NAT_H_

#include <boolmap.h>
#include <dataqueue.h>
#include <stddef.h>
#include "message.h"

#define NUM_AP_NAT_FLOWS 256

typedef struct {
  bool freeEntry : 1;
  bool recentlyUsed : 1;
  uint8_t protocol : 6;
  uint32_t sourceNode;
  uint32_t sourceIP;
  uint16_t sourcePort;
  uint32_t destinationIP;
  uint16_t destinationPort;
  uint8_t flag;
} gw_nat_flow_entry_t;  // 18 bytes

class GatewayNAT {
 private:
  gw_nat_flow_entry_t entries[NUM_AP_NAT_FLOWS];
  TaskHandle_t cleanNatTableTaskHandle;

 public:
  GatewayNAT();
  void translate(ipv4_datagram_t* datagram, uint32_t sourceNode);
  uint32_t revert(ipv4_datagram_t* datagram);
  void clean();
};

#endif
