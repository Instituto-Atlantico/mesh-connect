#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <dataqueue.h>
#include <layer2.h>
#include "message.h"

#define LORA_TASKS_CORE 0

class LoraMesh {
 private:
  TaskHandle_t taskHandle;
  DataQueue<message_t>* txQueue;
  DataQueue<message_t>* rxQueue;

 public:
  LoraMesh(DataQueue<message_t>* txQueue, DataQueue<message_t>* rxQueue);

  void transmit();
  void receive();
};

#endif
