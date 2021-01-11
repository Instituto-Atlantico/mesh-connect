#include <Arduino.h>
#include <display.h>

#define BLUE_LED 2

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

Display* display;

void setup() {
  Serial.begin(115200);
  pinMode(BLUE_LED, OUTPUT);

  display = new Display(OLED_SDA, OLED_SCL, OLED_RST, OLED_SCREEN_WIDTH,
                        OLED_SCREEN_HEIGHT);
  display->println("It works!");
  display->display();
}

void loop() {
  digitalWrite(BLUE_LED, HIGH);
  delay(1000);
  digitalWrite(BLUE_LED, LOW);
  delay(1000);
}
