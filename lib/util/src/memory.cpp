#include <Arduino.h>
#include <memory.h>

void printBuffer(void* buffer,
                 size_t size,
                 bool printSize,
                 const char* begin,
                 const char* end) {
  if (begin != nullptr)
    Serial.print(begin);

  for (int i = 0; i < size; i++) {
    Serial.printf("%02x", ((uint8_t*)buffer)[i]);
  }

  if (printSize)
    Serial.printf(" Size: %d", size);

  if (end != nullptr)
    Serial.print(end);
}
