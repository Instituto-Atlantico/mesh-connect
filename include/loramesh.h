#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <dataqueue.h>
#include <layer2.h>
#include "message.h"

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
