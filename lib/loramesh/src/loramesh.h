#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <dataqueue.h>

class LoraMesh {
 private:
  TaskHandle_t taskHandle;
  DataQueue* txQueue;
  DataQueue* rxQueue;

 public:
  LoraMesh(DataQueue* txQueue, DataQueue* rxQueue);

  void transmit();
  void receive();
};

#endif
