#ifndef _LED_H_
#define _LED_H_

#include <Arduino.h>

class Led {
 private:
  int pin;

 public:
  Led(int pin) : pin(pin) {
    if (pin >= 0)
      pinMode(pin, OUTPUT);
  }

  void on() {
    if (pin >= 0)
      digitalWrite(pin, HIGH);
  }

  void off() {
    if (pin >= 0)
      digitalWrite(pin, LOW);
  }
};

#endif
