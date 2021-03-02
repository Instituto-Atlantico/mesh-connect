#include <Arduino.h>
#include <dataqueue.h>
#include <display.h>
#include "ap.h"
#include "gateway.h"
#include "loramesh.h"

#define BLUE_LED 2

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

#define DATA_QUEUE_LENGTH 10
#define ACCESS_POINT_SSID "MeshConnect"
#define GATEWAY_SSID "MeshConnectGWAP"

Display* display;
DataQueue<message_t>* wifiToLoraQueue;
DataQueue<message_t>* loraToWifiQueue;
WifiNode* wifi;
LoraMesh* mesh;
// auto messageControl;
// message_t *messageData;

void setup() {
  Serial.begin(115200);
  pinMode(BLUE_LED, OUTPUT);
  digitalWrite(BLUE_LED, HIGH);

  display = new Display(OLED_SDA, OLED_SCL, OLED_RST, OLED_SCREEN_WIDTH,
                        OLED_SCREEN_HEIGHT);

  wifiToLoraQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);
  loraToWifiQueue = new DataQueue<message_t>(DATA_QUEUE_LENGTH);

  mesh = new LoraMesh(wifiToLoraQueue, loraToWifiQueue, new Router());
  // Testing Control message
  control_data_t controlData;
  controlData.type = GW_ANNOUNCEMENT;
  controlData.source = 12;
  auto messageControl = newControlMessage(controlData.type, controlData.source);
  wifiToLoraQueue->push(&messageControl);

  if (shouldEnableGateway(GATEWAY_SSID)) {
    wifi = new Gateway(GATEWAY_SSID, wifiToLoraQueue, loraToWifiQueue);
  } else {
    wifi = new AccessPoint(ACCESS_POINT_SSID, wifiToLoraQueue, loraToWifiQueue);
  }

  digitalWrite(BLUE_LED, LOW);
}

void loop() {
  digitalWrite(BLUE_LED, HIGH);
  display->clearDisplay();
  display->setCursor(0, 0);

  display->print("Mode: ");
  display->println(wifi->getMode());
  display->print("IP: ");
  display->println(wifi->getIPAddress());

  auto status = wifi->getStatus();
  display->print("RX Frames: ");
  display->println(status.rxFrames);
  display->print("RX Errors: ");
  display->println(status.rxErrors);
  display->print("RSSI: ");
  display->println(status.rssi);
  display->print("Len: ");
  display->println(status.length);

  display->print("Drops (L/W): ");
  display->print(wifiToLoraQueue->getStatus().drops);
  display->print("/");
  display->print(loraToWifiQueue->getStatus().drops);

  display->display();
  digitalWrite(BLUE_LED, LOW);
  delay(1000);
}
