#include "nat.h"
#include <Arduino.h>
#include <layer3.h>
#include <string.h>

#define GATNAT_TASK_CORE 1
#define PUBLIC_PORT_OFFSET 10000

static void cleanNatTableTask(void* pointer) {
  auto gatNAT = (GatewayNAT*)pointer;
  for (;;) {
    gatNAT->clean();
    vTaskDelay(60000 / portTICK_PERIOD_MS);
  }
}

GatewayNAT::GatewayNAT() {
  memset(entries, 0, sizeof(entries));
  for (auto& entry : entries) {
    entry.freeEntry = true;
  }

  xTaskCreatePinnedToCore(cleanNatTableTask, "CleanNatTableTaskHandle", 10000,
                          this, 0, &cleanNatTableTaskHandle, GATNAT_TASK_CORE);
}

void GatewayNAT::translate(ipv4_datagram_t* datagram, uint32_t sourceNode) {
  ipv4_headers_t* ipv4 = (ipv4_headers_t*)datagram->payload;
  size_t size = datagram->size;

  gw_nat_flow_entry_t entry = {.freeEntry = false,
                               .recentlyUsed = true,
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
    if (entries[index].destinationIP == entry.destinationIP &&
        entries[index].destinationPort == entry.destinationPort &&
        entries[index].freeEntry == entry.freeEntry &&
        entries[index].protocol == entry.protocol &&
        entries[index].sourceIP == entry.sourceIP &&
        entries[index].sourceNode == entry.sourceNode &&
        entries[index].sourcePort == entry.sourcePort) {
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

  entries[index].recentlyUsed = true;

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
  tableEntry->recentlyUsed = true;

  return tableEntry->sourceNode;
}

void GatewayNAT::clean() {
  for (auto& entry : entries) {
    if (!entry.freeEntry && entry.recentlyUsed) {
      entry.recentlyUsed = false;
    } else if (!entry.freeEntry && !entry.recentlyUsed) {
      entry.freeEntry = true;
    }
  }
}
