#include <Arduino.h>
#include <dataqueue.h>
#include <display.h>
#include <wifinode.h>
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

  // Control Message
  // control_data_t controlData;
  // controlData.type = GW_ANNOUNCEMENT;
  // controlData.source = 12;
  // auto messageControl = newControlMessage(controlData.type,
  // controlData.source); wifiToLoraQueue->push(&messageControl);

  // COM5 - Verde
  // COM6 - Vermelho

  // Data Message
  layer2_data_t layer2Data;
  layer2Data.type = ETHER_TYPE_UNKNOWN;
  // Serial.print("\nType: ");
  // Serial.print(layer2Data.type);
  uint8_t mac[6] = {0};

  WiFi.macAddress(mac);
  memcpy(layer2Data.source, mac, sizeof(uint8_t) * 6);
  // Serial.print("\nSource: ");
  for (int i = 0; i < 6; i++) {
    // Serial.print(layer2Data.source[i]);
  }

  uint8_t receiver[6] = {0xAC, 0x67, 0xB2, 0x24, 0x14, 0xBC};
  memcpy(layer2Data.destination, receiver, sizeof(receiver));
  // Serial.print("\nDestination: ");
  for (size_t i = 0; i < 6; i++) {
    // Serial.print(layer2Data.destination[i]);
  }

  uint8_t data[8] = {0xfe, 0xeb, 0xda, 0xed, 0xaa, 0xbb, 0xcc, 0xdd};    // FEDDAEBF
  layer2Data.payload = (uint8_t*)malloc(sizeof(data));
  memcpy(layer2Data.payload, &data, 8*sizeof(uint8_t));

  layer2Data.length = sizeof(data);
  // Serial.print("\nPayload: ");
  // Serial.printf("%X", *((int*)layer2Data.payload));

  // Serial.print("\nLength: ");
  // Serial.println(layer2Data.length);

  auto messageData = newDataMessage(layer2Data);
  wifiToLoraQueue->push(&messageData);
}
