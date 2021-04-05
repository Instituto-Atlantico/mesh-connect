#ifndef _MEMORY_H_
#define _MEMORY_H_

void printBuffer(void* buffer,
                 size_t size,
                 bool printSize = false,
                 const char* tag = nullptr,
                 const char* end = "\n");

#endif
