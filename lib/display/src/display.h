#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <Adafruit_SSD1306.h>

class Display : public Adafruit_SSD1306
{
public:
    Display(int sda, int scl, int rst, int width, int height);
};

#endif
