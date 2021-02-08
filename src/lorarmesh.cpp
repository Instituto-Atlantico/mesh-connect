#ifndef _LORAMESH_CPP_
#define _LORAMESH_CPP_

#include "loramesh.h"

static void task(void* pointer) {
  auto loraMesh = (LoraMesh*)pointer;
  for (;;) {
    loraMesh->receive();
    loraMesh->transmit();
  }
}

LoraMesh::LoraMesh(DataQueue<message_t>* txQueue,
                   DataQueue<message_t>* rxQueue) {
  this->txQueue = txQueue;
  this->rxQueue = rxQueue;
  xTaskCreatePinnedToCore(task, "LoraMesh", 10000, this, 0, &taskHandle, 0);
}

void LoraMesh::transmit() {
  auto message = txQueue->poll();
  if (message == nullptr)
    return;

  // send over LoRaLayer2... if control, then broadcasts. else lookup table

  if (message->type == DATA_MESSAGE) {
    free(message->data.layer2.payload);
  }
  free(message);
}

void LoraMesh::receive() {}

#endif
