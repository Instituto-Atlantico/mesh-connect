#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <inttypes.h>

typedef struct {
  uint8_t version : 4;
  uint8_t ihl : 4;
  uint8_t dscp : 6;
  uint8_t ecn : 2;
  uint16_t length : 16;
  uint16_t id : 16;
  uint8_t flags : 3;
  uint16_t offset : 13;
  uint8_t ttl : 8;
  uint8_t protocol : 8;
  uint16_t checksum : 16;
  uint8_t source[4];
  uint8_t destination[4];
} ipv4_headers_t;  // 20 bytes
#define IPV4_HEADERS_LEN sizeof(ipv4_headers_t)

#endif