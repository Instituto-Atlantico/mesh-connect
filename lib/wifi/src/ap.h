#ifndef _AP_H_
#define _AP_H_

#include <WiFi.h>
#include <dataqueue.h>
#include <esp_wifi.h>

typedef struct {
  uint32_t rxFrames;
  uint16_t rxErrors;
  int16_t rssi;
  uint16_t length;
} ap_status_t;

class AccessPoint {
 private:
  TaskHandle_t taskHandle;
  ap_status_t status;
  DataQueue* rxQueue;
  DataQueue* txQueue;

 public:
  AccessPoint(const char* ssid, DataQueue* rxQueue, DataQueue* txQueue);

  IPAddress getIPAddress();
  uint8_t getNumberOfClients();
  ap_status_t getStatus();

  void receive(wifi_promiscuous_pkt_t* packet);
  void transmit();
};

#endif
