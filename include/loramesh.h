#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <dataqueue.h>
#include <layer2.h>
#include "message.h"
#include "router.h"
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>

#define LORA_TASKS_CORE 0

class LoraMesh {
 private:
  TaskHandle_t taskHandle;
  DataQueue<message_t>* txQueue;
  DataQueue<message_t>* rxQueue;
  Router* router;
  Layer1Class* Layer1;
  LL2Class* LL2;
  char routes[256];
  char neighbors[256];

 public:
  LoraMesh(DataQueue<message_t>* txQueue,
           DataQueue<message_t>* rxQueue,
           Router* router);

  void transmit();
  void receive();
};

#endif
