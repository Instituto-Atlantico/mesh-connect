#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <dataqueue.h>
#include <layer2.h>
#include <wifinode.h>

#define SCAN_ATTEMPTS 3

bool shouldEnableGateway(const char* gwSSID, int scanAttempts = SCAN_ATTEMPTS);

class Gateway : public WifiNode {
 private:
  DataQueue<layer2_data_t>* rxQueue;
  DataQueue<layer2_data_t>* txQueue;

 public:
  Gateway(const char* gwSSID,
          DataQueue<layer2_data_t>* rxQueue,
          DataQueue<layer2_data_t>* txQueue);

  wifi_node_status_t getStatus() override;
  String getMode() override { return "GW"; }
};

#endif
