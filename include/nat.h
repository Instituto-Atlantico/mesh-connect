#ifndef _NAT_H_
#define _NAT_H_

#include <boolmap.h>
#include <stddef.h>
#include "message.h"

#define NUM_AP_NAT_FLOWS 256

typedef struct {
  bool freeEntry : 1;
  uint8_t protocol : 7;
  uint32_t sourceNode;
  uint32_t sourceIP;
  uint16_t sourcePort;
  uint32_t destinationIP;
  uint16_t destinationPort;
  bool flag;
} gw_nat_flow_entry_t;  // 18 bytes

class GatewayNAT {
 private:
  gw_nat_flow_entry_t entries[NUM_AP_NAT_FLOWS];

 public:
  GatewayNAT();
  void translate(ipv4_datagram_t* datagram, uint32_t sourceNode);
  uint32_t revert(ipv4_datagram_t* datagram);
  void clean();
};

#endif
