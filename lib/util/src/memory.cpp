#include <Arduino.h>
#include <memory.h>

void printBuffer(void* buffer,
                 size_t size,
                 bool printSize,
                 const char* tag,
                 const char* end) {
  if (tag != nullptr)
    Serial.print(tag);

  for (int i = 0; i < size; i++) {
    Serial.printf("%02x", ((uint8_t*)buffer)[i]);
  }

  if (printSize)
    Serial.printf(" Size: %d", size);

  if (end != nullptr)
    Serial.print(end);
}
