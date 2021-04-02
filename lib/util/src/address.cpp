#include <address.h>

String getLocalMACAddress() {
  return convertMACAddress(getLocalMACAddressAsUint32());
}

uint32_t getLocalMACAddressAsUint32() {
  return ESP.getEfuseMac() >> 16;
}

String convertMACAddress(uint32_t mac) {
  char string[9] = {0};

  sprintf(string, "%02x%02x%02x%02x", (mac & 0xFF000000) >> 24,
          (mac & 0xFF0000) >> 16, (mac & 0xFF00) >> 8, mac & 0xFF);

  return String(string);
}
