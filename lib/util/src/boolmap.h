#ifndef _BOOL_MAP_H_
#define _BOOL_MAP_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

class BoolMap {
 private:
  uint8_t* entries;
  uint32_t numEntries;

 public:
  BoolMap(uint32_t numEntries) {
    if (numEntries % 8 != 0) {
      throw "the number of entries must be multiple of 8";
    }
    this->numEntries = numEntries;
    this->entries = (uint8_t*)calloc(0, numEntries / 8);
  }

  bool get(uint32_t& i) {
    if (i >= numEntries)
      throw "IndexOutOfBoundsException";
    return entries[i / 8] & (1 << (i % 8));
  }

  void set(uint32_t& i, bool value) {
    if (i >= numEntries)
      throw "IndexOutOfBoundsException";
    if (value)
      entries[i / 8] |= (1 << (i % 8));
    else
      entries[i / 8] &= ~(1 << (i % 8));
  }

  uint32_t findFirstIndexWithTrue(bool value) {
    for (int e = 0; e < numEntries / 8; e++) {
      uint8_t entry = entries[e];
      if (entry > 0) {
        for (int i = 0; i < 8; i++) {
          if (entry >> i & 1) {
            return e * 8 + i;
          }
        }
      }
    }

    throw "NotFound";
  }

  ~BoolMap() { free(entries); }
};

#endif
