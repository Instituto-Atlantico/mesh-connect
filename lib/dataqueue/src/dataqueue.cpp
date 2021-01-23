#ifndef _DATAQUEUE_CPP_
#define _DATAQUEUE_CPP_

#include <dataqueue.h>
#include <cstring>  //for memcpy

DataQueue::DataQueue(int capacity) {
  queue = xQueueCreate(capacity, sizeof(layer2_data_t));
  if (queue == nullptr) {
    throw "Could not create the queue";
  }
  status.drops = 0;
}

void DataQueue::push(layer2_data_t* data) {
  auto result = xQueueSend(queue, data, 0);
  if (result == errQUEUE_FULL) {
    status.drops++;
  }
}

layer2_data_t DataQueue::remove() {
  layer2_data_t data;
  xQueueReceive(queue, &data, portMAX_DELAY);
  return data;
}

data_queue_status_t DataQueue::getStatus() {
  return status;
}

#endif
