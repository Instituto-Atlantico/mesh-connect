#ifndef _LED_H_
#define _LED_H_

class Led {
 private:
  int pin;

 public:
  Led(int pin);
  void on();
  void off();
};

#endif
