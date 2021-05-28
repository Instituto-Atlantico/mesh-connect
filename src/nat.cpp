#include "nat.h"
#include <Arduino.h>
#include <layer3.h>
#include <string.h>

#define PUBLIC_PORT_OFFSET 10000

GatewayNAT::GatewayNAT() {
  memset(entries, 0, sizeof(entries));
  for (auto& entry : entries) {
    entry.freeEntry = true;
  }

  //Array oo time
  memset(times, 0, sizeof(times));
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
                               .destinationPort = 0,
                               .flag = 1};
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
      entries[index].flag = 1;
      Serial.printf("I'm already in the table\n");
      Serial.printf("Flag %u\n", entries[index].flag);
      Serial.printf("Index %u\n", index);
      Serial.printf("SRC: %.2X\n", entries[index].sourceIP);
      Serial.printf("DST: %.2X\n", entries[index].destinationIP);
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
      Serial.printf("I'm enter in the table\n");
      Serial.printf("Flag %u\n", entries[index].flag);
      Serial.printf("Index %u\n", index);
      Serial.printf("SRC: %.2X\n", entries[index].sourceIP);
      Serial.printf("DST: %.2X\n", entries[index].destinationIP);
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
  int indice = 0;
  for (auto& entry : entries) {
    if (entry.destinationPort == sourcePort &&
        entry.destinationIP == ipv4->sourceIP &&
        entry.protocol == ipv4->protocol) {
      tableEntry = &entry;
    }
    indice++;
  }

  if (tableEntry == nullptr)
    throw "Invalid NAT port mapping";

  ipv4->destinationIP = tableEntry->sourceIP;
  setL4Port(ipv4, size, tableEntry->sourcePort, DESTINATION_PORT_INDEX);
  tableEntry->flag =0;
  // Catch time;
  if(tableEntry->flag == 0){
    times[indice] = esp_timer_get_time();
  }

  return tableEntry->sourceNode;
}

void GatewayNAT::clean() {
  uint8_t indice = 0;
  for (auto& entry : entries) {
    if (entry.freeEntry == false && entry.flag == 0) {
      entry.freeEntry = true;
      Serial.println("I will clean the table");
      Serial.printf("Flag %u\n", entry.flag);
      Serial.printf("Index %u\n", indice);
      Serial.printf("SRC : %.2X\n", entry.sourceIP);
      Serial.printf("DST : %.2X\n", entry.destinationIP);
      auto now = esp_timer_get_time();
      // Time to clean
      Serial.printf("Time to clean : %d\n", now - times[indice]);
    }
    indice++;
  }
}
