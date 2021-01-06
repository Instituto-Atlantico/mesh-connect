#ifndef _NODE_H_
#define _NODE_H_

#include <Arduino.h>
#include <display.h>

class MeshConnectNode
{
private:
    Display *display;

public:
    MeshConnectNode(Display *);
    Display *getDisplay();
    void doWork();
    unsigned int getSleepTime();
};

#endif
