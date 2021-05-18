#ifndef _NAT_H_
#define _NAT_H_
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define ABS(X) ((X)<0?(X*(-1)):(X))


#include <boolmap.h>
#include <stddef.h>
#include "message.h"

#define NUM_AP_NAT_FLOWS 256

typedef struct {
  uint8_t protocol;
  uint32_t destinationIP;
  uint16_t destinationPort;
  uint32_t sourceNode;
  uint32_t sourceIP;
  uint16_t sourcePort;
} gw_nat_flow_entry_t;  // 17 bytes


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
