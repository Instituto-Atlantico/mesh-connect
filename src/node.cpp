#ifndef _NODE_CPP_
#define _NODE_CPP_

#include <node.h>

MeshConnectNode::MeshConnectNode(Display *display)
{
    this->display = display;
}

Display *MeshConnectNode::getDisplay()
{
    return this->display;
}

void MeshConnectNode::doWork()
{
    display->clearDisplay();
    display->display();
}

unsigned int MeshConnectNode::getSleepTime()
{
    return 1000;
}

#endif
