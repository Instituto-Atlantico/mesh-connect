#ifndef _FRAGMENTER_H_
#define _FRAGMENTER_H_

#include <inttypes.h>
#include <stddef.h>

typedef struct {
  uint8_t id;
  uint8_t fragmentIndex;
  uint8_t fragmentSize;
  uint16_t totalSize;
  void* data;
} fragment_t;

class Fragments {
 private:
  static uint8_t nextId;

  uint8_t id;
  void* data;
  uint16_t totalSize;
  uint8_t maxFragmentSize;
  uint8_t count;
  uint8_t currentFragment;

 public:
  Fragments(void* data, uint16_t size, uint8_t maxFragmentSize);
  Fragments(const Fragments& other);
  bool hasNext();
  fragment_t next();
  void reset();
};

#endif
