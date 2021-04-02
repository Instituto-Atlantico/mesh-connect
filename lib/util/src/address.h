#ifndef _ADDRESS_H_
#define _ADDRESS_H_

#include <Esp.h>

String getLocalMACAddress();
uint32_t getLocalMACAddressAsUint32();
String convertMACAddress(uint32_t);

#endif
