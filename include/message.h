#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>

typedef enum __attribute__((packed)) {
  IPV4_DATAGRAM_MESSAGE = 0,
  CONTROL_MESSAGE,
} message_type_t;

typedef enum __attribute__((packed)) {
  GW_ANNOUNCEMENT = 0,
} control_data_type_t;

typedef struct __attribute__((packed)) {
  control_data_type_t type;
} control_data_t;

typedef struct __attribute__((packed)) {
  void* payload;
  uint8_t size;
} ipv4_datagram_t;

typedef struct __attribute__((packed)) {
  message_type_t type;

  uint32_t source;
  uint32_t destination;
  union {
    ipv4_datagram_t ipv4Datagram;
    control_data_t control;
  } data;
} message_t;

inline message_t newIPv4DataMessage(ipv4_datagram_t datagram,
                                    uint32_t source = 0,
                                    uint32_t destination = 0) {
  return message_t{.type = IPV4_DATAGRAM_MESSAGE,
                   .source = source,
                   .destination = destination,
                   .data = {.ipv4Datagram = datagram}};
};

inline message_t newControlMessage(control_data_type_t type) {
  return message_t{.type = CONTROL_MESSAGE,
                   .source = 0,
                   .destination = 0xFFFFFFFF,
                   .data = {.control = {.type = type}}};
};

#define MIN_MESSAGE_LENGTH 1  // control messages have 1 byte length

#endif
