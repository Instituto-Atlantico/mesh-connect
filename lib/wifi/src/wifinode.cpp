#ifndef _WIFINODE_CPP_
#define _WIFINODE_CPP_

#include <WiFi.h>
#include <wifinode.h>

WifiNode::WifiNode() {
  memset(&status, 0, sizeof(status));
}

IPAddress WifiNode::getIPAddress() {
  if (WiFi.getMode() == WIFI_MODE_APSTA)
    return WiFi.softAPIP();

  return WiFi.localIP();
}

uint8_t WifiNode::getNumberOfClients() {
  return WiFi.softAPgetStationNum();
}

wifi_node_status_t WifiNode::getStatus() {
  return this->status;
}

#endif
