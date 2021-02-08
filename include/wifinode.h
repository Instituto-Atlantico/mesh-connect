#ifndef _WIFINODE_H_
#define _WIFINODE_H_

#include <WiFi.h>
#include <inttypes.h>

#define WIFI_TASKS_CORE 1

typedef struct {
  uint32_t rxFrames;
  uint16_t rxErrors;
  uint32_t txFrames;
  uint16_t txErrors;
  int16_t rssi;
  uint16_t length;
} wifi_node_status_t;

class WifiNode {
 protected:
  wifi_node_status_t status;

 public:
  WifiNode();

  IPAddress getIPAddress();
  uint8_t getNumberOfClients();
  virtual wifi_node_status_t getStatus();

  virtual String getMode() = 0;
};

#endif
