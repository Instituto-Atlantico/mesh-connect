#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include "loramesh.h"

#define ADDR_LENGTH 4
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LORA_FREQ 915E6
#define LED 25
#define TX_POWER 20

#define DATAGRAM_HEADER 5
#define MESSAGE_LENGTH 233

const int BROADCAST_NODES[ADDR_LENGTH] = {0xff, 0xff, 0xff, 0xff};

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

  Layer1 = new Layer1Class();
  Layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  Layer1->setTxPower(TX_POWER);
  Layer1->setLoRaFrequency(LORA_FREQ);
  int newTry = 0;
  while (!Layer1->init() or newTry < 30) {
    newTry++;
  }
  if (newTry < 30) {
    LL2 = new LL2Class(Layer1);
    LL2->setInterval(10000);  // set to zero to disable routing packets
    LL2->init();
  } else {
    delay(500);
    ESP.restart();
  }
  
  xTaskCreatePinnedToCore(task, "LoraMeshTransceiver", 10000, this, 0,
                          &transceiverTaskHandle, LORA_TASKS_CORE);
  
}

void LoraMesh::transmit() {
  auto message = txQueue->poll();

  if (message == nullptr)
    return;

  LL2->daemon();
  int datagramsize = 0;
  struct Datagram datagram;
  if (message->type == CONTROL_MESSAGE) {
    memcpy(datagram.destination, BROADCAST_NODES, ADDR_LENGTH);
    datagram.type = message->type;
    memcpy(datagram.message, &message->data.control, sizeof(control_data_t));
    datagramsize = DATAGRAM_HEADER;
    datagramsize += sizeof(control_data_t);

  } else if (message->type == DATA_MESSAGE) {
    auto destinationAddr = router->getGatewayAddress();
    if (destinationAddr > 0) {
      datagram.type = message->type;
      memcpy(datagram.destination, &destinationAddr, ADDR_LENGTH);
      memcpy(datagram.message, message->data.layer2.payload,
             message->data.layer2.length);
      datagramsize = DATAGRAM_HEADER;
      datagramsize += message->data.layer2.length;

    } else {
      goto freeData;
    }
  }

  LL2->writeData(datagram, datagramsize);

freeData:
  if (message->type == DATA_MESSAGE) {
    free(message->data.layer2.payload);
  }
  free(message);
}

void LoraMesh::receive() {
  message_t* message = nullptr;
  // TODO read actual data from LoRaLayer2
}
