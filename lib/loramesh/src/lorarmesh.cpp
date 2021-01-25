#ifndef _LORAMESH_CPP_
#define _LORAMESH_CPP_

#include <loramesh.h>

static void task(void* pointer) {
  auto loraMesh = (LoraMesh*)pointer;
  for (;;) {
    loraMesh->receive();
    loraMesh->transmit();
  }
}

LoraMesh::LoraMesh(DataQueue* txQueue, DataQueue* rxQueue) {
  this->txQueue = txQueue;
  this->rxQueue = rxQueue;
  xTaskCreatePinnedToCore(task, "LoraMesh", 10000, this, 0, &taskHandle, 0);
}

void LoraMesh::transmit() {
  auto layer2Data = txQueue->poll();
  if (layer2Data == nullptr)
    return;

  // send over LoRaLayer2

  free(layer2Data->payload);
  free(layer2Data);
}

void LoraMesh::receive() {}

#endif
