#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include <dataqueue.h>
#include <layer2.h>
#include "message.h"
#include "router.h"

#define LORA_TASKS_CORE 0

class LoraMesh {
 private:
  TaskHandle_t transceiverTaskHandle;
  DataQueue<message_t>* txQueue;
  DataQueue<message_t>* rxQueue;
  Router* router;
  Layer1Class* layer1;
  LL2Class* layer2;

 public:
  LoraMesh(DataQueue<message_t>* txQueue,
           DataQueue<message_t>* rxQueue,
           Router* router);

  void transmit();
  void receive();
};

#endif
