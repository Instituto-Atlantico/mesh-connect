#include <Arduino.h>
#include <memory.h>
#include <packetprint.h>

void printLayer2Data(layer2_data_t* layer2Data) {
  if (layer2Data == nullptr)
    return;

  printBuffer(&layer2Data->destination, 6, false, "DST MAC: ");
  printBuffer(&layer2Data->source, 6, false, "SRC MAC: ");

  switch (layer2Data->type) {
    case ETHER_TYPE_IPV4: {
      printIPv4Data((ipv4_headers_t*)layer2Data->payload, layer2Data->length);
      break;
    }
    case ETHER_TYPE_IPV6:
    case ETHER_TYPE_ARP:
    case ETHER_TYPE_UNKNOWN:
      Serial.printf("Unsupported ether type: 0x%04X\n", layer2Data->type);
      break;
  }
}

void printIPv4Data(ipv4_headers_t* ipv4Data, size_t size) {
  Serial.printf("DST IP:\t%s\n",
                IPAddress(ipv4Data->destinationIP).toString().c_str());
  Serial.printf("SRC IP:\t%s\n",
                IPAddress(ipv4Data->sourceIP).toString().c_str());
  Serial.printf("PROTO:\t%d\n", ipv4Data->protocol);

  size_t ipv4HeaderLength = getIPv4HeaderLength(ipv4Data);
  size_t dataSize = size - ipv4HeaderLength;
  void* dataStart = ((uint8_t*)ipv4Data) + ipv4HeaderLength;

  printBuffer(dataStart, dataSize, true, "DATA:\t");
}
