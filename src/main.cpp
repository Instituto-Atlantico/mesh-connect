#include <Arduino.h>

#define BLUE_LED 2

void setup() {
  Serial.begin(115200);
  pinMode(BLUE_LED, OUTPUT);
}

void loop() {
  digitalWrite(BLUE_LED, HIGH);
  delay(1000);
  digitalWrite(BLUE_LED, LOW);
  delay(1000);
}
