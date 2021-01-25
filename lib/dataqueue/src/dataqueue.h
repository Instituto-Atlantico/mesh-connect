#ifndef _DATAQUEUE_H_
#define _DATAQUEUE_H_

#include <FreeRTOS.h>
#include <freertos/queue.h>

typedef struct {
  // TODO implement inRate and outRate
  uint32_t drops;
} data_queue_status_t;

template <typename T>
class DataQueue {
 private:
  QueueHandle_t queue;
  data_queue_status_t status;

 public:
  DataQueue(int capacity) {
    queue = xQueueCreate(capacity, sizeof(T));
    if (queue == nullptr) {
      throw "Could not create the queue";
    }
    status.drops = 0;
  };

  void push(T* data) {
    auto result = xQueueSend(queue, data, 0);
    if (result == errQUEUE_FULL) {
      status.drops++;
    }
  };

  T* poll() {
    if (uxQueueMessagesWaiting(queue) == 0)
      return nullptr;

    T* data = (T*)malloc(sizeof(T));
    if (data == nullptr)
      return nullptr;

    auto result = xQueueReceive(queue, data, 0);
    if (result == pdFALSE) {
      free(data);
      return nullptr;
    }

    return data;
  };

  data_queue_status_t getStatus() { return status; };
};

#endif
