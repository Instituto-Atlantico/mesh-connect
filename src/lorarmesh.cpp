#include "loramesh.h"
#include <Layer1_LoRa.h>
#include <LoRaLayer2.h>

#define ADDR_LENGTH 4
#define LORA_CS 18
#define LORA_RST 14
#define LORA_IRQ 26

#define LORA_FREQ 915E6
#define LED 25
#define TX_POWER 20
#define DATAGRAM_HEADER 5
#undef LL2_DEBUG
uint8_t BROADCAST_NODES[ADDR_LENGTH] = {0xff, 0xff, 0xff, 0xff};


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
  if (Layer1->init()) 
  {
    Serial.println(" --> Layer1 initialized");
    LL2 = new LL2Class(Layer1);  // initialize Layer2
    LL2->setLocalAddress("c0d3f00d");  // this should either be randomized or set using
                                // the wifi mac address
    LL2->setInterval(10000);    // set to zero to disable routing packets
    if (LL2->init() != 0) {
      ESP.restart();
    } 
  }
  xTaskCreatePinnedToCore(task, "LoraMesh", 10000, this, 0, &taskHandle,
                          LORA_TASKS_CORE);
  
}

void LoraMesh::transmit() {
  auto message = txQueue->poll();

  if (message == nullptr)
    return;
 
  if (message->type == CONTROL_MESSAGE) {
    int datagramsize = 0;
    struct Datagram datagram;
    memcpy(datagram.destination, BROADCAST_NODES, ADDR_LENGTH); 
    datagram.type = message->type;
    memcpy(datagram.message, &message->data.control, sizeof(control_data_t));
    datagramsize = DATAGRAM_HEADER;  
    datagramsize += sizeof(control_data_t); 
    LL2->daemon(); 
    LL2->writeData(datagram, datagramsize);
    // TODO broadcast message over LoRaLayer2
  } else if (message->type == DATA_MESSAGE) {
    auto destinationAddr = router->getGatewayAddress();
    if (destinationAddr > 0){
      int datagramsize = 0;
      struct Datagram datagram;
      datagram.type = message->type;
      memcpy(datagram.destination, &destinationAddr, ADDR_LENGTH); 
      memcpy(datagram.message, message->data.layer2.payload, message->data.layer2.length);
      datagramsize = DATAGRAM_HEADER;  
      datagramsize += message->data.layer2.length;
      LL2->daemon(); 
      LL2->writeData(datagram, datagramsize);
  
      // TODO send to destinationAddr through LoRaLayer2
    }
    
  }
  
  if (message->type == DATA_MESSAGE) {
    free(message->data.layer2.payload);
  }
  free(message);
}

void LoraMesh::receive() {
  //message_t* message = nullptr;  // TODO read actual data from LoRaLayer2
  auto message = rxQueue->poll();
  if (message == nullptr)
    return;
  
  if (message->type == CONTROL_MESSAGE) {
    router->handleControlMessage(message);
  }
}
