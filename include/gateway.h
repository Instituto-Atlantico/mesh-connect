#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <dataqueue.h>
#include <layer2.h>
#include "message.h"
#include "wifinode.h"

#define SCAN_ATTEMPTS 3

bool shouldEnableGateway(const char* gwSSID,
                         const char* gwPassword,
                         int scanAttempts = SCAN_ATTEMPTS);

class Gateway : public WifiNode {
 private:
  TaskHandle_t announceTaskHandle;
  DataQueue<message_t>* rxQueue;
  DataQueue<message_t>* txQueue;

 public:
  Gateway(const char* gwSSID,
          const char* gwPassword,
          DataQueue<message_t>* rxQueue,
          DataQueue<message_t>* txQueue);

  wifi_node_status_t getStatus() override;
  String getMode() override { return "GW"; }

  void announce();
};

#endif
