#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include <address.h>
#include <wifinode.h>
#include "loramesh.h"

#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LORA_FREQ 915E6
#define TX_POWER 20

#define DATAGRAM_HEADER 5
#define ROUTE_UPDATE_INTERVAL 10000  // 10 seconds
#define MIN_PACKET_LENGTH (HEADER_LENGTH + DATAGRAM_HEADER + MIN_MESSAGE_LENGTH)

#define BOOT_LL1_DELAY_MICROS 500
#define MAX_BOOT_LL1_RETRIES 10

static void task(void* pointer) {
  auto loraMesh = (LoraMesh*)pointer;
  for (;;) {
    loraMesh->transmit();
    loraMesh->receive();
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

  layer2 = new LL2Class(layer1);
  layer2->setLocalAddress(getLocalMACAddress().c_str());
  layer2->setInterval(ROUTE_UPDATE_INTERVAL);
  layer2->init();

  xTaskCreatePinnedToCore(task, "LoraMeshTransceiver", 10000, this, 0,
                          &transceiverTaskHandle, LORA_TASKS_CORE);
}

void LoraMesh::transmit() {
  layer2->daemon();

  auto message = txQueue->poll();
  if (message == nullptr)
    return;

  struct Datagram datagram;
  datagram.type = message->type;
  int datagramSize = 0;

  if (message->type == CONTROL_MESSAGE) {
    memcpy(datagram.destination, BROADCAST, ADDR_LENGTH);
    memcpy(datagram.message, &message->data.control, sizeof(control_data_t));
    datagramSize += sizeof(control_data_t);
  }

  else if (message->type == DATA_MESSAGE) {
    auto destinationAddr = router->getGatewayAddress();

    if (destinationAddr > 0) {
      memcpy(datagram.destination, &destinationAddr, ADDR_LENGTH);
      memcpy(datagram.message, &message->data.layer2, LAYER2_DATA_HEADERS_LEN);
      memcpy(datagram.message + LAYER2_DATA_HEADERS_LEN,
             message->data.layer2.payload, message->data.layer2.length);
      datagramSize += LAYER2_DATA_HEADERS_LEN + message->data.layer2.length;
    }
  }

  if (datagramSize > 0) {
    layer2->writeData(datagram, DATAGRAM_HEADER + datagramSize);
  }

  if (message->type == DATA_MESSAGE) {
    free(message->data.layer2.payload);
  }

  free(message);
}

void LoraMesh::receive() {
  struct Packet packet = layer2->readData();
  if (packet.totalLength <= MIN_PACKET_LENGTH) {
    return;
  }

  if (packet.datagram.type == CONTROL_MESSAGE) {
    control_data_t controlData;
    memcpy(&controlData, packet.datagram.message, sizeof(control_data_t));
    message_t message = newControlMessage(controlData);

    router->handleControlMessage(&message);
  }

  else if (packet.datagram.type == DATA_MESSAGE) {
    layer2_data_t layer2Data;
    memcpy(&layer2Data, packet.datagram.message, LAYER2_DATA_HEADERS_LEN);

    layer2Data.payload = malloc(layer2Data.length);
    memcpy(layer2Data.payload,
           packet.datagram.message + LAYER2_DATA_HEADERS_LEN,
           layer2Data.length);

    message_t message = newDataMessage(layer2Data);
    rxQueue->push(&message);
  }
}
