#ifndef _DATAQUEUE_H_
#define _DATAQUEUE_H_

#include <FreeRTOS.h>
#include <freertos/queue.h>
#include <layer2.h>

typedef struct {
  // uint16_t inRate;
  // uint16_t outRate;
  uint32_t drops;
} data_queue_status_t;

class DataQueue {
 private:
  QueueHandle_t queue;
  data_queue_status_t status;

 public:
  DataQueue(int capacity);

  void push(layer2_data_t*);
  layer2_data_t* poll();

  data_queue_status_t getStatus();
};

#endif
