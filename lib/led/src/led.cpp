#include <Arduino.h>
#include <led.h>

Led::Led(int pin) : pin(pin) {
  if (pin >= 0)
    pinMode(pin, OUTPUT);
}

void Led::on() {
  if (pin >= 0)
    digitalWrite(pin, HIGH);
}

void Led::off() {
  if (pin >= 0)
    digitalWrite(pin, LOW);
}
