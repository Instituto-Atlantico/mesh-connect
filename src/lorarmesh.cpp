#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include "loramesh.h"

#define ADDR_LENGTH 4
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LORA_FREQ 915E6
#define TX_POWER 20

#define DATAGRAM_HEADER 5

#define BOOT_LL1_DELAY_MICROS 500
#define MAX_BOOT_LL1_RETRIES 10

static int BROADCAST_NODES[ADDR_LENGTH] = {0xff, 0xff, 0xff, 0xff};

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

  layer1 = new Layer1Class();
  layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  layer1->setTxPower(TX_POWER);
  layer1->setLoRaFrequency(LORA_FREQ);

  int attempts = MAX_BOOT_LL1_RETRIES;

  while (!layer1->init()) {
    delay(BOOT_LL1_DELAY_MICROS);
    if (attempts-- == 0)
      ESP.restart();
  }
  ll2 = new LL2Class(layer1);
  ll2->setInterval(10000);  // this value needs more research
  ll2->init();

  xTaskCreatePinnedToCore(task, "LoraMeshTransceiver", 10000, this, 0,
                          &transceiverTaskHandle, LORA_TASKS_CORE);
}

void LoraMesh::transmit() {
  auto message = txQueue->poll();

  if (message == nullptr)
    return;

  ll2->daemon();
  struct Datagram datagram;
  int datagramsize = DATAGRAM_HEADER;
  datagram.type = message->type;
  bool shouldTransmitt = false;

  if (message->type == CONTROL_MESSAGE) {
    memcpy(datagram.destination, BROADCAST_NODES, ADDR_LENGTH);
    memcpy(datagram.message, &message->data.control, sizeof(control_data_t));
    datagramsize += sizeof(control_data_t);
    shouldTransmitt = true;

  } else if (message->type == DATA_MESSAGE) {
    auto destinationAddr = router->getGatewayAddress();
    if (destinationAddr > 0) {
      memcpy(datagram.destination, &destinationAddr, ADDR_LENGTH);
      memcpy(datagram.message, message->data.layer2.payload,
             message->data.layer2.length);
      datagramsize += message->data.layer2.length;
      shouldTransmitt = true;
    }
  }

  if (shouldTransmitt) {
    ll2->writeData(datagram, datagramsize);
  }

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