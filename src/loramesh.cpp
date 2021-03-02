#include <Arduino.h>
#include "loramesh.h"
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include <wifinode.h>

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
  Serial.println(message->data);
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

  } else if (message->type == DATA_MESSAGE &&
             (LAYER2_DATA_HEADERS_LEN + message->data.layer2.length) <
                 WIFI_NODE_MTU) {
    auto destinationAddr = router->getGatewayAddress();
    if (destinationAddr > 0) {
      memcpy(datagram.destination, &destinationAddr, ADDR_LENGTH);

      memcpy(datagram.message, &message->data.layer2, LAYER2_DATA_HEADERS_LEN);

      memcpy((datagram.message + LAYER2_DATA_HEADERS_LEN),
             message->data.layer2.payload, message->data.layer2.length);

      datagramsize += LAYER2_DATA_HEADERS_LEN + message->data.layer2.length;
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
  message_t* message = nullptr;
  ll2->daemon();
  struct Packet packet = ll2->readData();
  // When buffer return 0, that means the packet have a size 0
  if (packet.totalLength == 0)
    return;

  if (packet.datagram.type == CONTROL_MESSAGE) {
    control_data_type_t controlDataType;
    uint32_t source;
    memcpy(&controlDataType, packet.datagram.message,
           sizeof(control_data_type_t));
    memcpy(&source, packet.datagram.message + sizeof(control_data_type_t),
           sizeof(uint32_t));
    *message = newControlMessage(controlDataType, source);
    router->handleControlMessage(message);

  } else if (packet.datagram.type ==
             DATA_MESSAGE) {  // if the device is the gateway
    layer2_data_t layer2Data;
    memcpy(&layer2Data, packet.datagram.message, LAYER2_DATA_HEADERS_LEN);
    memcpy(layer2Data.payload,
           packet.datagram.message + LAYER2_DATA_HEADERS_LEN,
           layer2Data.length);
    *message = newDataMessage(layer2Data);
  } else {
    return;
  }
  rxQueue->push(message);
}