#ifndef _AP_H_
#define _AP_H_

#include <WiFi.h>
#include <dataqueue.h>
#include <esp_wifi.h>
#include <layer2.h>
#include "message.h"
#include "wifinode.h"

class AccessPoint : public WifiNode {
 private:
  TaskHandle_t taskHandle;
  DataQueue<message_t>* rxQueue;
  DataQueue<message_t>* txQueue;

 public:
  AccessPoint(const char* ssid,
              DataQueue<message_t>* rxQueue,
              DataQueue<message_t>* txQueue);

  void receive(wifi_promiscuous_pkt_t* packet);
  void transmit();

  String getMode() { return "AP"; }
};

#endif
