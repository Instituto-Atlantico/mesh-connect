#ifndef _AP_H_
#define _AP_H_

#include <esp_wifi.h>
#include "wifinode.h"

class AccessPoint : public WifiNode {
 public:
  AccessPoint(const char* ssid,
              DataQueue<message_t>* rxQueue,
              DataQueue<message_t>* txQueue);
  String getMode() override;

  void receive(wifi_promiscuous_pkt_t* packet);
  void transmit() override;
};

#endif
