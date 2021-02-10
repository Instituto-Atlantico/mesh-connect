#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "message.h"

class Router {
 private:
  uint32_t lastGatewayAddress;
  int64_t lastGatewaySeenTime;

 public:
  Router();
  void handleControlMessage(message_t* message);
  uint32_t getGatewayAddress();
};

#endif
