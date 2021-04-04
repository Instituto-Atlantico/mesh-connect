#include "monitor.h"
#include <Arduino.h>
#include <address.h>

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

DisplayMonitor::DisplayMonitor(DataQueue<message_t>* wifiToLoraQueue,
                               DataQueue<message_t>* loraToWifiQueue,
                               WifiNode* wifi,
                               LoraMesh* mesh) {
  this->wifiToLoraQueue = wifiToLoraQueue;
  this->loraToWifiQueue = loraToWifiQueue;
  this->wifi = wifi;
  this->mesh = mesh;
  this->display = new Display(OLED_SDA, OLED_SCL, OLED_RST, OLED_SCREEN_WIDTH,
                              OLED_SCREEN_HEIGHT);
};

void DisplayMonitor::updateInfo() {
  display->clearDisplay();
  display->setCursor(0, 0);

  display->print(wifi->getMode());
  display->print(F(" MAC: "));
  display->println(getLocalMACAddress());
  display->print(F("IP: "));
  display->println(wifi->getIPAddress());

  auto status = wifi->getStatus();
  display->print(F("RX Frames: "));
  display->println(status.rxFrames);
  display->print(F("RX Errors: "));
  display->println(status.rxErrors);
  display->print(F("RSSI: "));
  display->println(status.rssi);
  display->print(F("Len: "));
  display->println(status.length);

  display->print(F("Drops (L/W): "));
  display->print(wifiToLoraQueue->getStatus().drops);
  display->print(F("/"));
  display->print(loraToWifiQueue->getStatus().drops);

  display->display();
}

LogMonitor::LogMonitor(DataQueue<message_t>* wifiToLoraQueue,
                       DataQueue<message_t>* loraToWifiQueue,
                       WifiNode* wifi,
                       LoraMesh* mesh) {
  this->wifiToLoraQueue = wifiToLoraQueue;
  this->loraToWifiQueue = loraToWifiQueue;
  this->wifi = wifi;
  this->mesh = mesh;
};

void LogMonitor::updateInfo() {
  Serial.println(F("========================================================"));
  Serial.print(wifi->getMode());
  Serial.print(F(" IP: "));
  Serial.print(wifi->getIPAddress());
  Serial.print(F(" MAC: "));
  Serial.println(getLocalMACAddress());

  auto status = wifi->getStatus();
  Serial.print(F("WiFi Status - RX Frames: "));
  Serial.print(status.rxFrames);
  Serial.print(F(" RX Errors: "));
  Serial.print(status.rxErrors);
  Serial.print(F(" RSSI: "));
  Serial.print(status.rssi);
  Serial.print(F(" Len: "));
  Serial.println(status.length);

  Serial.print(F("Queue Drops - WifiToLora/LoraToWifi: "));
  Serial.print(wifiToLoraQueue->getStatus().drops);
  Serial.print(F("/"));
  Serial.println(loraToWifiQueue->getStatus().drops);
  Serial.println(F("========================================================"));
}
