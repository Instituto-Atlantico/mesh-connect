#ifndef _ROUTER_H_
#define _ROUTER_H_

#include "loramesh.h"
#include "message.h"
#include "nat.h"
#include "wifinode.h"

class Router {
 private:
  TaskHandle_t routerTaskHandle;
  WifiNode* wifiNode;
  LoraMesh* loraMesh;

 protected:
  virtual bool routeMessageFromLoRa(message_t* message) = 0;
  virtual bool routeMessageFromWiFi(message_t* message) = 0;

 public:
  Router(WifiNode* wifiNode, LoraMesh* loraMesh);
  void handleMessagesFromLoRa();
  void handleMessagesFromWiFi();
};

class AcessPointRouter : public Router {
 private:
  uint32_t lastGatewayAddress;
  int64_t lastGatewaySeenTime;

 protected:
  bool routeMessageFromLoRa(message_t* message);
  bool routeMessageFromWiFi(message_t* message);

 public:
  AcessPointRouter(WifiNode* wifiNode, LoraMesh* loraMesh);
  uint32_t getGatewayAddress();
};

class GatewayRouter : public Router {
 private:
  GatewayNAT nat;

 protected:
  bool routeMessageFromLoRa(message_t* message);
  bool routeMessageFromWiFi(message_t* message);

 public:
  GatewayRouter(WifiNode* wifiNode, LoraMesh* loraMesh);
  void cleanNatTable();
};

#endif
