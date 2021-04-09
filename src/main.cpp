#include <Arduino.h>
#include <dataqueue.h>
#include <led.h>
#include "ap.h"
#include "gateway.h"
#include "loramesh.h"
#include "monitor.h"
#include "wifinode.h"

// Observability defines
#ifndef MONITOR_CLASS
#define MONITOR_CLASS LogMonitor
#endif
#ifndef MONITOR_UPDATE_INTERVAL
#define MONITOR_UPDATE_INTERVAL 1000
#endif
#ifndef BLINK_LED
#define BLINK_LED -1
#endif

// Operational defines
#ifndef DATA_QUEUE_LENGTH
#define DATA_QUEUE_LENGTH 10
#endif
#ifndef ACCESS_POINT_SSID
#define ACCESS_POINT_SSID "MeshConnect"
#endif
#ifndef GATEWAY_SSID
#define GATEWAY_SSID "MeshConnectGWAP"
#endif
#ifndef GATEWAY_SSID_PASSWORD
#define GATEWAY_SSID_PASSWORD nullptr
#endif

DataQueue<message_t>* wifiToLoraQueue;
DataQueue<message_t>* loraToWifiQueue;
WifiNode* wifi;
LoraMesh* mesh;
Monitor* monitor;
Led led(BLINK_LED);

void setup() {
  Serial.begin(115200);
  led.on();

  wifiToLoraQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);
  loraToWifiQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);

  mesh = new LoraMesh(wifiToLoraQueue, loraToWifiQueue, new Router());

  if (shouldEnableGateway(GATEWAY_SSID, GATEWAY_SSID_PASSWORD)) {
    wifi = new Gateway(GATEWAY_SSID, GATEWAY_SSID_PASSWORD, wifiToLoraQueue,
                       loraToWifiQueue);
  } else {
    wifi = new AccessPoint(ACCESS_POINT_SSID, wifiToLoraQueue, loraToWifiQueue);
  }

  monitor = new MONITOR_CLASS(wifiToLoraQueue, loraToWifiQueue, wifi, mesh);

  led.off();
}

void loop() {
  led.on();
  monitor->updateInfo();
  led.off();
  delay(MONITOR_UPDATE_INTERVAL);
}
