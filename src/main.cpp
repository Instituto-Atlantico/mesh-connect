#include <Arduino.h>
#include <ap.h>
#include <dataqueue.h>
#include <display.h>

#define BLUE_LED 2

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

#define DATA_QUEUE_LENGTH 10
#define ACCESS_POINT_SSID "MeshConnect"

Display* display;
DataQueue* queue;
AccessPoint* ap;

void setup() {
  Serial.begin(115200);
  pinMode(BLUE_LED, OUTPUT);

  display = new Display(OLED_SDA, OLED_SCL, OLED_RST, OLED_SCREEN_WIDTH,
                        OLED_SCREEN_HEIGHT);
  queue = new DataQueue(DATA_QUEUE_LENGTH);
  ap = new AccessPoint(ACCESS_POINT_SSID, queue);
}

void loop() {
  digitalWrite(BLUE_LED, HIGH);
  display->clearDisplay();
  display->setCursor(0, 0);

  display->print("IP: ");
  display->println(ap->getIPAddress());
  display->print("Clients: ");
  display->println(ap->getNumberOfClients());

  auto status = ap->getStatus();
  display->print("RX Frames: ");
  display->println(status.rxFrames);
  display->print("RX Errors: ");
  display->println(status.rxErrors);
  display->print("Avg RSSI: ");
  display->println(status.rssi);
  display->print("Avg Len: ");
  display->println(status.length);

  auto queueStatus = queue->getStatus();
  display->print("Queue Drops: ");
  display->println(queueStatus.drops);

  display->display();
  digitalWrite(BLUE_LED, LOW);
  delay(1000);
}
