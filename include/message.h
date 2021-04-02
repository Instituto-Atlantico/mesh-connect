#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <layer2.h>

typedef enum __attribute__((packed)) {
  DATA_MESSAGE = 0,
  CONTROL_MESSAGE,
} message_type_t;

typedef enum __attribute__((packed)) {
  GW_ANNOUNCEMENT = 0,
} control_data_type_t;

typedef struct __attribute__((packed)) {
  control_data_type_t type;
  uint32_t source;
} control_data_t;

typedef struct __attribute__((packed)) {
  message_type_t type;

  union {
    layer2_data_t layer2;
    control_data_t control;
  } data;
} message_t;

inline message_t newDataMessage(layer2_data_t data) {
  return message_t{.type = DATA_MESSAGE, .data = {.layer2 = data}};
};

inline message_t newControlMessage(control_data_t controlData) {
  return message_t{.type = CONTROL_MESSAGE, .data = {.control = controlData}};
};

inline message_t newControlMessage(control_data_type_t type, uint32_t source) {
  return message_t{.type = CONTROL_MESSAGE,
                   .data = {.control = {.type = type, .source = source}}};
};

#define MIN_MESSAGE_LENGTH                                                  \
  sizeof(control_data_t) < LAYER2_DATA_HEADERS_LEN ? sizeof(control_data_t) \
                                                   : LAYER2_DATA_HEADERS_LEN

#endif
