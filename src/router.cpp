#include "router.h"
#include <esp_timer.h>

#define GW_ANNOUNCEMENT_DEADLINE 6 * 10e6  // 1 minute in microseconds

Router::Router() {
  lastGatewayAddress = 0;
  lastGatewaySeenTime = 0;
}

void Router::handleControlMessage(message_t* message) {
  if (message->data.control.type == GW_ANNOUNCEMENT) {
    lastGatewayAddress = message->data.control.source;
    lastGatewaySeenTime = esp_timer_get_time();
  }
}

uint32_t Router::getGatewayAddress() {
  auto now = esp_timer_get_time();
  if (now - lastGatewaySeenTime > GW_ANNOUNCEMENT_DEADLINE)
    return 0;

  return lastGatewayAddress;
}
