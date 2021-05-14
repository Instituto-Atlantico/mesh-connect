#ifndef _LAYER3_H_
#define _LAYER3_H_

#include <inttypes.h>
#include <stddef.h>

#define SOURCE_PORT_INDEX 0
#define DESTINATION_PORT_INDEX 1

enum protocol_numbers {
  ICMP = 1,
  TCP = 6,
  UDP = 17,
};

typedef struct {
  uint8_t ihl : 4;
  uint8_t version : 4;
  uint8_t ecn : 2;
  uint8_t dscp : 6;
  uint16_t length : 16;
  uint16_t id : 16;
  uint16_t fragmentation : 16;
  uint8_t ttl : 8;
  uint8_t protocol : 8;
  uint16_t checksum : 16;
  uint32_t sourceIP;
  uint32_t destinationIP;
} ipv4_headers_t;  // 20 bytes
#define IPV4_HEADERS_LEN sizeof(ipv4_headers_t)

bool isMulticast(uint32_t ip);
uint8_t getIPv4HeaderLength(ipv4_headers_t* headers);
void getL4PortNumbers(ipv4_headers_t* ipv4,
                      size_t size,
                      uint16_t* sourcePort,
                      uint16_t* destinationPort);
void setL4Port(ipv4_headers_t* ipv4,
               size_t size,
               uint16_t port,
               uint8_t portIndex);
bool getDontFragmentBit(ipv4_headers_t* headers);

#endif
