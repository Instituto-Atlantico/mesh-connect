#ifndef _LORAMESH_H_
#define _LORAMESH_H_

#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include <dataqueue.h>
#include <fragmenter.h>
#include "message.h"

#define LORA_TASKS_CORE 0

class LoraMesh {
 private:
  TaskHandle_t transceiverTaskHandle;
  DataQueue<message_t>* txQueue;
  DataQueue<message_t>* rxQueue;
  Layer1Class* layer1;
  LL2Class* layer2;

  Reasembler* reasemblers[1][256];

  void transmitControlMessage(message_t* message);
  void transmitDataMessage(message_t* message);
  void transmitFragmentedDataMessage(message_t* message);
  void receiveControlMessage(struct Packet& packet);
  void receiveDataMessage(struct Packet& packet);
  void receiveFragmentedDataMessage(struct Packet& packet);

 public:
  LoraMesh(DataQueue<message_t>* txQueue, DataQueue<message_t>* rxQueue);

  void transmit();
  void receive();

  DataQueue<message_t>* getTXQueue();
  DataQueue<message_t>* getRXQueue();
};

#endif
