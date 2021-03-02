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

  // Testing Data Message
  layer2_data_t layer2Data;
  layer2Data.type = ETHER_TYPE_UNKNOWN;
  Serial.print("Type: ");
  Serial.println(layer2Data.type);
  memset(layer2Data.source, 1, sizeof(uint8_t) * 6);
  Serial.println("Source: ");
  for (int i = 0; i < 6; i++) {
    Serial.println(layer2Data.source[i]);
  }
  Serial.println("Destination: ");
  memset(layer2Data.destination, 1, sizeof(uint8_t) * 6);
  for (int i = 0; i < 6; i++) {
    Serial.println(layer2Data.destination[i]);
  }
  layer2Data.length = 10;
  Serial.print("Length: ");
  Serial.println(layer2Data.length);
  uint8_t data = 10;
  layer2Data.payload = &data;
  Serial.print("Payload: ");
  if (layer2Data.payload == nullptr) {
    Serial.println("Sucess");
  } else {
    Serial.println("Fail");
  }
  // This part of the code is failing
  auto messageData = newDataMessage(layer2Data);
  wifiToLoraQueue->push(&messageData);

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
