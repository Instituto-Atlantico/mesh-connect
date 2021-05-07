#include "nat.h"
#include <layer3.h>
#include <string.h>

#define PUBLIC_PORT_OFFSET 10000

GatewayNAT::GatewayNAT() {
  memset(entries, 0, sizeof(entries));
  for (auto& entry : entries) {
    entry.freeEntry = true;
  }
}

void GatewayNAT::translate(ipv4_datagram_t* datagram, uint32_t sourceNode) {
  ipv4_headers_t* ipv4 = (ipv4_headers_t*)datagram->payload;
  size_t size = datagram->size;

  gw_nat_flow_entry_t entry = {.freeEntry = false,
                               .protocol = ipv4->protocol,
                               .sourceNode = sourceNode,
                               .sourceIP = ipv4->sourceIP,
                               .sourcePort = 0,
                               .destinationIP = ipv4->destinationIP,
                               .destinationPort = 0};
  getL4PortNumbers(ipv4, size, &entry.sourcePort, &entry.destinationPort);

  uint16_t index = 0;
  int16_t freeIndex = -1;
  for (; index < NUM_AP_NAT_FLOWS; index++) {
    if (memcmp(&entries[index], &entry, sizeof(entry)) == 0) {
      break;
    } else if (freeIndex == -1 && entries[index].freeEntry) {
      freeIndex = index;
    }
  }

  if (index == NUM_AP_NAT_FLOWS) {
    if (freeIndex == -1) {
      throw "No NAT table entry available";
    } else {
      index = freeIndex;
      entries[index] = entry;
    }
  }

  if (ipv4->protocol != ICMP) {
    uint16_t publicPort = index + PUBLIC_PORT_OFFSET;
    setL4Port(ipv4, size, publicPort, SOURCE_PORT_INDEX);
  }
}

uint32_t GatewayNAT::revert(ipv4_datagram_t* datagram) {
  ipv4_headers_t* ipv4 = (ipv4_headers_t*)datagram->payload;
  size_t size = datagram->size;

  uint16_t sourcePort;
  uint16_t destinationPort;
  getL4PortNumbers(ipv4, size, &sourcePort, &destinationPort);

  gw_nat_flow_entry_t* tableEntry = nullptr;
  for (auto& entry : entries) {
    if (entry.destinationPort == sourcePort &&
        entry.destinationIP == ipv4->sourceIP &&
        entry.protocol == ipv4->protocol) {
      tableEntry = &entry;
    }
  }

  if (tableEntry == nullptr)
    throw "Invalid NAT port mapping";

  ipv4->destinationIP = tableEntry->sourceIP;
  setL4Port(ipv4, size, tableEntry->sourcePort, DESTINATION_PORT_INDEX);

  return tableEntry->sourceNode;
}

void GatewayNAT::clean() {
  printf("Plase implement me\n");
}
