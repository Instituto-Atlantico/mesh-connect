#ifndef _AP_H_
#define _AP_H_

#include <WiFi.h>
#include <esp_wifi.h>

typedef struct {
  uint32_t rxFrames;
  uint16_t rxErrors;
  int16_t rssi;
  uint16_t length;
} ap_status_t;

class AccessPoint {
 private:
  ap_status_t status;

 public:
  AccessPoint(const char* ssid);

  IPAddress getIPAddress();
  uint8_t getNumberOfClients();
  ap_status_t getStatus();

  void handleIncomingDataPacket(wifi_promiscuous_pkt_t* packet);
};

#endif
