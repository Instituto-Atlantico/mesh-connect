#include "loramesh.h"
#include <Arduino.h>
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>
#include <wifinode.h>

#define ADDR_LENGTH 4
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26
#define LORA_FREQ 915E6
#define TX_POWER 20
#define LL2_DEBUG 1

#define DATAGRAM_HEADER 5

#define BOOT_LL1_DELAY_MICROS 500
#define MAX_BOOT_LL1_RETRIES 10

uint8_t LOCAL_ADDRESS[ADDR_LENGTH] = {0};
// static int BROADCAST[ADDR_LENGTH] = {0xff, 0xff, 0xff, 0xff};
const static int padding = 2;

static void task(void* pointer) {
  auto loraMesh = (LoraMesh*)pointer;
  for (;;) {
    loraMesh->receive();
    // loraMesh->transmit();
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
  ll2->setLocalAddress(generatingLocalMac(LOCAL_ADDRESS));
  ll2->setInterval(10000);  // this value needs more research
  ll2->init();

  xTaskCreatePinnedToCore(task, "LoraMeshTransceiver", 10000, this, 0,
                          &transceiverTaskHandle, LORA_TASKS_CORE);
}

char* LoraMesh::generatingLocalMac(uint8_t* mac) {
  WiFi.macAddress(mac);
  char* macStr = (char*)malloc(sizeof(char) * 16);
  sprintf_P(macStr, "%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3]);
  return macStr;
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
    memcpy(datagram.destination, BROADCAST, ADDR_LENGTH);
    memcpy(datagram.message, &message->data.control, sizeof(control_data_t));
    datagramsize += sizeof(control_data_t);
    shouldTransmitt = true;
    Serial.println("Control Message transmited");
    // Serial.printf("Control Message :");
    // for(int i = 0; i<233; i++){
    //   Serial.printf("%02X",datagram.message[i]);
    // }
    // Serial.printf("\n");

  } else if (message->type == DATA_MESSAGE &&
             (LAYER2_DATA_HEADERS_LEN + message->data.layer2.length) <
                 WIFI_NODE_MTU) {
    // auto destinationAddr = router->getGatewayAddress();
    auto destinationAddr = 1;
      if (destinationAddr > 0) {
      // memcpy(datagram.destination, &destinationAddr, ADDR_LENGTH);
      uint8_t receiver[4] = {0x24, 0x6f, 0x28, 0x99};//24 6F 28 99
      memcpy(datagram.destination, receiver, ADDR_LENGTH);
      memcpy(datagram.message, &message->data.layer2, LAYER2_DATA_HEADERS_LEN);
      // const static int padding = 2;
      memcpy((datagram.message + LAYER2_DATA_HEADERS_LEN - padding),
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

  if (packet.totalLength == 0) {
    // Serial.println("Everything is empty");
    return;
  }
  if (packet.datagram.type == CONTROL_MESSAGE) {
    control_data_type_t controlDataType;
    uint32_t source;
    // Serial.println("Print Control message ");
    // Serial.println("");
    memcpy(&controlDataType, packet.datagram.message,
           sizeof(control_data_type_t));
    memcpy(&source, packet.datagram.message + sizeof(control_data_type_t),
           sizeof(uint32_t));
    message = (message_t*)malloc(sizeof(packet.datagram.message));
    *message = newControlMessage(controlDataType, source);
    Serial.println("Control Datas type: ");
    Serial.printf("Type : %02X\n", message->data.control.type);
    Serial.printf("Source : %02X\n", message->data.control.source);
    
    Serial.println("Control Message received");
    router->handleControlMessage(message);
  } 
  
  else if (packet.datagram.type ==
             DATA_MESSAGE) {  // if the device is the gateway
    message = (message_t*)malloc(sizeof(packet.datagram.message));
    layer2_data_t layer2Data;
    Serial.printf("Datagram : ");
    for(int i = 0; i<233; i++){
      Serial.printf("%02X",packet.datagram.message[i]);
    }
    Serial.println("");

    memcpy(&layer2Data, packet.datagram.message, LAYER2_DATA_HEADERS_LEN);
    layer2Data.payload = malloc(layer2Data.length);
    // Serial.println("Depos do do 1 Memcpy");
    memcpy(layer2Data.payload,
           packet.datagram.message + LAYER2_DATA_HEADERS_LEN - padding,
           layer2Data.length);
    *message = newDataMessage(layer2Data);
    Serial.printf("Type: %02X\n", (message->data.layer2.type));
    Serial.printf("Lenght: %02X\n", message->data.layer2.length);
    Serial.print("Source: ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", message->data.layer2.source[i]);
    }
    Serial.println("");
    Serial.print("Destination: ");
    for (int i = 0; i < 6; i++) {
      Serial.printf("%02X", message->data.layer2.destination[i]);
    }
    Serial.println("");
    Serial.printf("Payload: ");
    for(int i = 0; i < message->data.layer2.length; i++)
      printf("[%d] = %02X\n", i ,((uint8_t*) message->data.layer2.payload)[i]);
    Serial.println("");
  }
  else {
    Serial.println("Empty type");
    return;
  }
   if (message != nullptr) {
    
    rxQueue->push(message); 
    
  }


}