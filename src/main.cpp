#include <Arduino.h>
#include <dataqueue.h>
#include <led.h>
#include "ap.h"
#include "gateway.h"
#include "loramesh.h"
#include "monitor.h"
#include "router.h"
#include "wifinode.h"

// Observability defines
#ifndef MONITOR_CLASS
#define MONITOR_CLASS LogMonitor
#endif
#ifndef MONITOR_UPDATE_INTERVAL
#define MONITOR_UPDATE_INTERVAL 1000
#endif
#ifndef BLINK_LED 14
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

WifiNode* wifi;
LoraMesh* mesh;
Router* router;
Monitor* monitor;
Led led(BLINK_LED);

void setup() {
  Serial.begin(115200);
  led.on();

  auto loraTXQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);
  auto loraRXQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);
  mesh = new LoraMesh(loraTXQueue, loraRXQueue);

  auto wifiTXQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);
  auto wifiRXQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);

  if (shouldEnableGateway(GATEWAY_SSID, GATEWAY_SSID_PASSWORD)) {
    wifi = new Gateway(GATEWAY_SSID, GATEWAY_SSID_PASSWORD, wifiTXQueue,
                       wifiRXQueue);
    router = new GatewayRouter(wifi, mesh);
  } else {
    wifi = new AccessPoint(ACCESS_POINT_SSID, wifiTXQueue, wifiRXQueue);
    router = new AcessPointRouter(wifi, mesh);
  }

  monitor = new MONITOR_CLASS(loraTXQueue, loraRXQueue, wifi, mesh);  // FIXME

  led.off();
}

void loop() {
  led.on();
  monitor->updateInfo();
  led.off();
  delay(MONITOR_UPDATE_INTERVAL);
}
