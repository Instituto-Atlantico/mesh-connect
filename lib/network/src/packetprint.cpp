#include <Arduino.h>
#include <memory.h>
#include <packetprint.h>

#define IPV4_HEADER_LEN_MULTIPLIER 4

void printLayer2Data(layer2_data_t* layer2Data) {
  if (layer2Data == nullptr)
    return;

  printBuffer(&layer2Data->destination, 6, false, "DST MAC: ", "\n");
  printBuffer(&layer2Data->source, 6, false, "SRC MAC: ", "\n");

  switch (layer2Data->type) {
    case ETHER_TYPE_IPV4: {
      ipv4_headers_t* ipv4 = (ipv4_headers_t*)layer2Data->payload;
      Serial.printf("DST IP:\t%s\n",
                    IPAddress(ipv4->destinationIP).toString().c_str());
      Serial.printf("SRC IP:\t%s\n",
                    IPAddress(ipv4->sourceIP).toString().c_str());
      Serial.printf("PROTO:\t%d\n", ipv4->protocol);

      size_t ipv4HeaderLength = IPV4_HEADER_LEN_MULTIPLIER * ipv4->ihl;
      size_t dataSize = layer2Data->length - ipv4HeaderLength;
      void* dataStart = ((uint8_t*)layer2Data->payload) + ipv4HeaderLength;
      printBuffer(dataStart, dataSize, true, "DATA:\t", "\n");
      break;
    }
    case ETHER_TYPE_IPV6:
    case ETHER_TYPE_ARP:
    case ETHER_TYPE_UNKNOWN:
      Serial.printf("Unsupported ether type: 0x%04X\n", layer2Data->type);
      break;
  }
}
