#include "router.h"
#include <esp_timer.h>

#define ROUTER_TASK_CORE WIFI_TASKS_CORE
#define GW_ANNOUNCEMENT_DEADLINE 6 * 10e6  // 1 minute

static void routerTask(void* pointer) {
  auto router = (Router*)pointer;
  for (;;) {
    router->handleMessagesFromLoRa();
    router->handleMessagesFromWiFi();
  }
}

Router::Router(WifiNode* wifiNode, LoraMesh* loraMesh) {
  this->wifiNode = wifiNode;
  this->loraMesh = loraMesh;
  xTaskCreatePinnedToCore(routerTask, "RouterTask", 10000, this, 0,
                          &routerTaskHandle, ROUTER_TASK_CORE);
}

void Router::handleMessagesFromWiFi() {
  DataQueue<message_t>* wifiRXQueue = wifiNode->getRXQueue();
  DataQueue<message_t>* loraTXQueue = loraMesh->getTXQueue();

  message_t* message = wifiRXQueue->poll();
  if (message == nullptr)
    return;

  bool shouldForward = routeMessageFromWiFi(message);
  if (shouldForward)
    loraTXQueue->push(message);

  free(message);
}

void Router::handleMessagesFromLoRa() {
  DataQueue<message_t>* loraRXQueue = loraMesh->getRXQueue();
  DataQueue<message_t>* wifiTXQueue = wifiNode->getTXQueue();

  message_t* message = loraRXQueue->poll();
  if (message == nullptr)
    return;

  bool shouldForward = routeMessageFromLoRa(message);
  if (shouldForward)
    wifiTXQueue->push(message);

  free(message);
}

/*
**********************
****** APRouter ******
**********************
*/
AcessPointRouter::AcessPointRouter(WifiNode* wifiNode, LoraMesh* loraMesh)
    : Router(wifiNode, loraMesh) {
  this->lastGatewayAddress = 0;
  this->lastGatewaySeenTime = 0;
}

bool AcessPointRouter::routeMessageFromLoRa(message_t* message) {
  bool shouldForward = false;

  if (message->type == CONTROL_MESSAGE) {
    if (message->data.control.type == GW_ANNOUNCEMENT) {
      lastGatewayAddress = message->source;
      lastGatewaySeenTime = esp_timer_get_time();
    }
  } else if (message->type == IPV4_DATAGRAM_MESSAGE) {
    shouldForward = true;
  }

  return shouldForward;
}

bool AcessPointRouter::routeMessageFromWiFi(message_t* message) {
  bool shouldForward = false;

  if (message->type == IPV4_DATAGRAM_MESSAGE) {
    message->destination = getGatewayAddress();
    shouldForward = message->destination != 0;
  }

  return shouldForward;
}

uint32_t AcessPointRouter::getGatewayAddress() {
  auto now = esp_timer_get_time();
  if (now - lastGatewaySeenTime > GW_ANNOUNCEMENT_DEADLINE)
    return 0;

  return lastGatewayAddress;
}

/*
***************************
****** GatewayRouter ******
***************************
*/

static void gatewayRouterTask(void* pointer) {
  auto gatRouter = (GatewayRouter*)pointer;
  for (;;) {
    gatRouter->cleanNatTable();
  }
}
GatewayRouter::GatewayRouter(WifiNode* wifiNode, LoraMesh* loraMesh)
    : Router(wifiNode, loraMesh) {
  xTaskCreatePinnedToCore(gatewayRouterTask, "GatewayRouterTask", 10000, this, 0,
                          &GatewayRouterTaskHandle, ROUTER_TASK_CORE);
}

bool GatewayRouter::routeMessageFromLoRa(message_t* message) {
  bool shouldForward = false;

  if (message->type == IPV4_DATAGRAM_MESSAGE) {
    try {
      nat.translate(&message->data.ipv4Datagram, message->source);
      shouldForward = true;
    } catch (...) {
    }
  }

  return shouldForward;
}

bool GatewayRouter::routeMessageFromWiFi(message_t* message) {
  bool shouldForward = true;

  if (message->type == IPV4_DATAGRAM_MESSAGE) {
    try {
      message->destination = nat.revert(&message->data.ipv4Datagram);
    } catch (...) {
      shouldForward = false;
    }
  } else if (message->type == CONTROL_MESSAGE) {
    message->destination = 0xFFFFFFFF;
  }

  return shouldForward;
}

void GatewayRouter::cleanNatTable() {
  nat.clean();
}
