#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdio.h>

void printBuffer(void* buffer,
                 size_t size,
                 bool printSize = false,
                 const char* begin = nullptr,
                 const char* end = nullptr);

#endif
