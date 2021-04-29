#include <layer3.h>

#define MULTICAST_IP_RANGE_START 224
#define MULTICAST_IP_RANGE_END 239
#define IPV4_HEADER_SIZE_MULTIPLIER 4
#define TCPUDP_PORT_HEADERS_SIZE 8

bool isMulticast(uint32_t ip) {
  uint8_t firstByte = *((uint8_t*)&ip);
  return firstByte >= MULTICAST_IP_RANGE_START &&
         firstByte <= MULTICAST_IP_RANGE_END;
}

uint8_t getIPv4HeaderLength(ipv4_headers_t* headers) {
  return headers->ihl * IPV4_HEADER_SIZE_MULTIPLIER;
}

void getL4PortNumbers(ipv4_headers_t* ipv4,
                      size_t size,
                      uint16_t* sourcePort,
                      uint16_t* destinationPort) {
  if (ipv4->protocol == TCP || ipv4->protocol == UDP) {
    auto headerLength = getIPv4HeaderLength(ipv4);
    if (size < headerLength + TCPUDP_PORT_HEADERS_SIZE)
      throw "Cannot read L4 ports from IPv4 payload, insufficient size";

    uint16_t* portHeaders = (uint16_t*)(((uint8_t*)ipv4) + headerLength);
    *sourcePort = portHeaders[0];
    *destinationPort = portHeaders[1];
  } else {
    *sourcePort = 0;
    *destinationPort = 0;
  }
}

void setL4Port(ipv4_headers_t* ipv4,
               size_t size,
               uint16_t port,
               uint8_t portIndex) {
  if (portIndex > 1)
    throw "Port index must be either 0 or 1";

  if (ipv4->protocol == TCP || ipv4->protocol == UDP) {
    auto headerLength = getIPv4HeaderLength(ipv4);
    if (size < headerLength + TCPUDP_PORT_HEADERS_SIZE)
      throw "Cannot read L4 ports from IPv4 payload, insufficient size";

    uint16_t* portHeaders = (uint16_t*)(((uint8_t*)ipv4) + headerLength);
    portHeaders[portIndex] = port;
  }
}
