#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <dataqueue.h>
#include <display.h>
#include "loramesh.h"
#include "wifinode.h"

class Monitor {
 protected:
  DataQueue<message_t>* wifiToLoraQueue;
  DataQueue<message_t>* loraToWifiQueue;
  WifiNode* wifi;
  LoraMesh* mesh;

 public:
  virtual void updateInfo() = 0;
};

class DisplayMonitor : public Monitor {
 private:
  Display* display;

 public:
  DisplayMonitor(DataQueue<message_t>*,
                 DataQueue<message_t>*,
                 WifiNode*,
                 LoraMesh*);
  void updateInfo();
};

class LogMonitor : public Monitor {
 public:
  LogMonitor(DataQueue<message_t>*,
             DataQueue<message_t>*,
             WifiNode*,
             LoraMesh*);
  void updateInfo();
};

#endif
