#include "loramesh.h"

static void task(void* pointer) {
  auto loraMesh = (LoraMesh*)pointer;
  for (;;) {
    loraMesh->receive();
    loraMesh->transmit();
  }
}

LoraMesh::LoraMesh(DataQueue<message_t>* txQueue,
                   DataQueue<message_t>* rxQueue,
                   Router* router) {
  this->txQueue = txQueue;
  this->rxQueue = rxQueue;
  this->router = router;
  xTaskCreatePinnedToCore(task, "LoraMesh", 10000, this, 0, &taskHandle,
                          LORA_TASKS_CORE);
}

void LoraMesh::transmit() {
  auto message = txQueue->poll();
  if (message == nullptr)
    return;

  if (message->type == CONTROL_MESSAGE) {
    // TODO broadcast message over LoRaLayer2
  } else if (message->type == DATA_MESSAGE) {
    auto destinationAddr = router->getGatewayAddress();
    if (destinationAddr == 0)
      goto freeData;

    // TODO send to destinationAddr through LoRaLayer2
  }

freeData:
  if (message->type == DATA_MESSAGE) {
    free(message->data.layer2.payload);
  }
  free(message);
}

void LoraMesh::receive() {
  message_t* message = nullptr;  // TODO read actual data from LoRaLayer2
  if (message == nullptr)
    return;

  if (message->type == CONTROL_MESSAGE) {
    router->handleControlMessage(message);
  }
}
