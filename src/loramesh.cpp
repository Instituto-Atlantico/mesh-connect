#include "loramesh.h"
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include <address.h>
#include <fragmenter.h>
#include <wifinode.h>

#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LORA_FREQ 915E6
#define LORA_TX_POWER 20
#define LORA_SPREADING_FACTOR 7
#define LORA_DUTY_CYCLE 1.0

#define DATAGRAM_HEADER 5
#define ROUTE_UPDATE_INTERVAL 60000  // 60 seconds
#define MIN_PACKET_LENGTH (HEADER_LENGTH + DATAGRAM_HEADER + MIN_MESSAGE_LENGTH)

#define BOOT_LL1_DELAY_MICROS 500
#define MAX_BOOT_LL1_RETRIES 10

#define FRAG_DATA 50

static void task(void* pointer) {
  auto loraMesh = (LoraMesh*)pointer;
  for (;;) {
    loraMesh->transmit();
    loraMesh->receive();
  }
}

LoraMesh::LoraMesh(DataQueue<message_t>* txQueue,
                   DataQueue<message_t>* rxQueue) {
  this->txQueue = txQueue;
  this->rxQueue = rxQueue;

  layer1 = new Layer1Class();
  layer1->setPins(LORA_CS, LORA_RST, LORA_IRQ);
  layer1->setTxPower(LORA_TX_POWER);
  layer1->setLoRaFrequency(LORA_FREQ);
  layer1->setSpreadingFactor(LORA_SPREADING_FACTOR);

  int attempts = MAX_BOOT_LL1_RETRIES;
  while (!layer1->init()) {
    delay(BOOT_LL1_DELAY_MICROS);
    if (attempts-- == 0)
      ESP.restart();
  }

  layer2 = new LL2Class(layer1);
  layer2->setLocalAddress(getLocalMACAddress().c_str());
  layer2->setInterval(ROUTE_UPDATE_INTERVAL);
  layer2->setDutyCycle(LORA_DUTY_CYCLE);
  layer2->init();

  xTaskCreatePinnedToCore(task, "LoraMeshTransceiver", 10000, this, 0,
                          &transceiverTaskHandle, LORA_TASKS_CORE);
}

void LoraMesh::transmit() {
  layer2->daemon();

  auto message = txQueue->poll();
  if (message == nullptr || message->destination == 0) {
    free(message);
    return;
  }

  if (message->type == CONTROL_MESSAGE) {
    transmitControlMessage(message);
  } else if (message->type == IPV4_DATAGRAM_MESSAGE) {
    transmitDataMessage(message);
  }

  free(message);
}

void LoraMesh::transmitControlMessage(message_t* message) {
  int messageLength = sizeof(control_data_t);

  struct Datagram datagram;
  datagram.type = message->type;
  memcpy(datagram.message, &message->data.control, messageLength);
  memcpy(datagram.destination, &message->destination, ADDR_LENGTH);

  layer2->writeData(datagram, DATAGRAM_HEADER + messageLength);
}

void LoraMesh::transmitDataMessage(message_t* message) {
  auto messageLength = message->data.ipv4Datagram.size;
  auto payload = message->data.ipv4Datagram.payload;

  if (messageLength <= MESSAGE_LENGTH) {
    struct Datagram datagram;
    datagram.type = message->type;
    memcpy(datagram.message, payload, messageLength);
    free(payload);
    memcpy(datagram.destination, &message->destination, ADDR_LENGTH);
    layer2->writeData(datagram, DATAGRAM_HEADER + messageLength);
  }

  else {
    Fragments fragments(payload, messageLength, MESSAGE_LENGTH - 4);
    while (fragments.hasNext()) {
      auto fragment = fragments.next();

      struct Datagram datagram;
      datagram.type = FRAG_DATA;

      memcpy(datagram.message, &fragment.id, 1);
      memcpy(datagram.message + 1, &fragment.fragmentIndex, 1);
      memcpy(datagram.message + 2, &fragment.totalSize, 2);
      memcpy(datagram.message + 4, fragment.data, fragment.fragmentSize);
      memcpy(datagram.destination, &message->destination, ADDR_LENGTH);
      layer2->writeData(datagram, DATAGRAM_HEADER + messageLength);
    }

    free(payload);
  }
}

void LoraMesh::receive() {
  struct Packet packet = layer2->readData();
  if (packet.totalLength < MIN_PACKET_LENGTH) {
    return;
  }

  message_t message;
  memcpy(&message.source, packet.source, sizeof(uint32_t));
  memcpy(&message.destination, packet.datagram.destination, sizeof(uint32_t));

  if (packet.datagram.type == CONTROL_MESSAGE) {
    message.type = CONTROL_MESSAGE;
    memcpy(&message.data.control, packet.datagram.message,
           sizeof(control_data_t));
  }

  else if (packet.datagram.type == IPV4_DATAGRAM_MESSAGE) {
    message.type = IPV4_DATAGRAM_MESSAGE;

    message.data.ipv4Datagram.size =
        packet.totalLength - HEADER_LENGTH - DATAGRAM_HEADER;

    message.data.ipv4Datagram.payload = malloc(message.data.ipv4Datagram.size);
    memcpy(message.data.ipv4Datagram.payload, packet.datagram.message,
           message.data.ipv4Datagram.size);
  }

  rxQueue->push(&message);
}

DataQueue<message_t>* LoraMesh::getTXQueue() {
  return txQueue;
}
DataQueue<message_t>* LoraMesh::getRXQueue() {
  return rxQueue;
}
