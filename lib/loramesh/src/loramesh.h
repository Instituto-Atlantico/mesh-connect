#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <dataqueue.h>
#include <layer2.h>

class LoraMesh {
 private:
  TaskHandle_t taskHandle;
  DataQueue<layer2_data_t>* txQueue;
  DataQueue<layer2_data_t>* rxQueue;

 public:
  LoraMesh(DataQueue<layer2_data_t>* txQueue,
           DataQueue<layer2_data_t>* rxQueue);

  void transmit();
  void receive();
};

#endif
