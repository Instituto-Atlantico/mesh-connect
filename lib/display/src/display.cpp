#include <Wire.h>
#include <display.h>

Display::Display(int sda, int scl, int rst, int width, int height)
    : Adafruit_SSD1306(width, height, &Wire, rst) {
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);
  delay(20);
  digitalWrite(rst, HIGH);

  Wire.begin(sda, scl);
  if (!begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false))
    throw "Display initialization failed";

  clearDisplay();
  setTextColor(WHITE);
  setTextSize(1);
  display();
}
