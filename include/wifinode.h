#ifndef _WIFINODE_H_
#define _WIFINODE_H_

#include <WiFi.h>
#include <dataqueue.h>
#include <layer3.h>
#include "message.h"

#define WIFI_TASKS_CORE 1
#define WIFI_NODE_MTU (232)

typedef struct {
  uint32_t rxFrames;
  uint16_t rxErrors;
  uint32_t txFrames;
  uint16_t txErrors;
  int16_t rssi;
  uint16_t length;
} wifi_node_status_t;

class WifiNode {
 private:
  TaskHandle_t wifiTaskHandle;

 protected:
  wifi_node_status_t status;
  DataQueue<message_t>* txQueue;
  DataQueue<message_t>* rxQueue;

  void sendFragmentationNeeded(ipv4_headers_t* sourcePacket);
  void sendPacket(ipv4_headers_t* headers, size_t size, bool keepLocalIP);

 public:
  WifiNode(DataQueue<message_t>* txQueue, DataQueue<message_t>* rxQueue);

  IPAddress getIPAddress();
  uint8_t getNumberOfClients();

  virtual wifi_node_status_t getStatus();
  DataQueue<message_t>* getTXQueue();
  DataQueue<message_t>* getRXQueue();

  virtual void transmit() = 0;
  virtual String getMode() = 0;
};

#endif
