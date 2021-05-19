#include <fragmenter.h>

uint8_t Fragments::nextId = 0;

Fragments::Fragments(void* data, uint16_t size, uint8_t maxFragmentSize) {
  this->id = nextId++;
  this->data = data;
  this->totalSize = totalSize;
  this->maxFragmentSize = maxFragmentSize;
  this->count = size / maxFragmentSize + (size % maxFragmentSize != 0);
  this->currentFragment = 0;
}

Fragments::Fragments(const Fragments& other) {
  this->id = other.id;
  this->data = other.data;
  this->totalSize = other.totalSize;
  this->maxFragmentSize = other.maxFragmentSize;
  this->count = other.count;
  this->currentFragment = other.currentFragment;
}

bool Fragments::hasNext() {
  return currentFragment < count;
}

fragment_t Fragments::next() {
  if (!hasNext())
    throw "No more fragments to read";

  return fragment_t{
      .id = id,
      .fragmentIndex = currentFragment++,
      .fragmentSize = maxFragmentSize,
      .totalSize = totalSize,
      .data = ((uint8_t*)data) + (currentFragment * maxFragmentSize)};
}

void Fragments::reset() {
  this->currentFragment = 0;
}
