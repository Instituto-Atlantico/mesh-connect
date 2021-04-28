#ifndef _PACKETPRINT_H_
#define _PACKETPRINT_H_

#include <layer2.h>
#include <layer3.h>

void printLayer2Data(layer2_data_t* layer2Data);
void printIPv4Data(ipv4_headers_t* ipv4Data, size_t size);

#endif
