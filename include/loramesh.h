#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <dataqueue.h>
#include <layer2.h>
#include "message.h"
#include "router.h"

#define LORA_TASKS_CORE 0

class LoraMesh {
 private:
  TaskHandle_t taskHandle;
  DataQueue<message_t>* txQueue;
  DataQueue<message_t>* rxQueue;
  Router* router;

 public:
  LoraMesh(DataQueue<message_t>* txQueue,
           DataQueue<message_t>* rxQueue,
           Router* router);

  void transmit();
  void receive();
};

#endif
