#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <layer2.h>

enum message_type_t {
  DATA_MESSAGE = 0,
  CONTROL_MESSAGE,
};

enum control_data_type_t {
  GW_ANNOUNCEMENT = 0,
};

typedef struct {
  control_data_type_t type;
  uint32_t source;
} control_data_t;

typedef struct {
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
