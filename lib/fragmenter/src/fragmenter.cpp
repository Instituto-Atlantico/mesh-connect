#include <fragmenter.h>
#include <stdlib.h>
#include <string.h>

uint8_t Fragmenter::nextId = 0;

Fragmenter::Fragmenter(void* data, uint16_t size, uint8_t maxFragmentSize) {
  this->id = nextId++;
  this->data = data;
  this->totalSize = totalSize;
  this->maxFragmentSize = maxFragmentSize;
  this->count = size / maxFragmentSize + (size % maxFragmentSize != 0);
  this->currentFragment = 0;
}

Fragmenter::Fragmenter(const Fragmenter& other) {
  this->id = other.id;
  this->data = other.data;
  this->totalSize = other.totalSize;
  this->maxFragmentSize = other.maxFragmentSize;
  this->count = other.count;
  this->currentFragment = other.currentFragment;
}

bool Fragmenter::hasNext() {
  return currentFragment < count;
}

fragment_t Fragmenter::next() {
  if (!hasNext())
    throw "No more fragments to read";

  return fragment_t{
      .id = id,
      .fragmentIndex = currentFragment++,
      .fragmentSize = maxFragmentSize,
      .totalSize = totalSize,
      .data = ((uint8_t*)data) + (currentFragment * maxFragmentSize)};
}

void Fragmenter::reset() {
  this->currentFragment = 0;
}

Reasembler::Reasembler(fragment_t fragment, uint8_t maxFragmentSize) {
  memset(receivedFragments, false, sizeof(receivedFragments));
  id = fragment.id;
  data = calloc(fragment.totalSize, sizeof(uint8_t));
  totalSize = fragment.totalSize;
  this->maxFragmentSize = maxFragmentSize;
  count = totalSize / maxFragmentSize + (totalSize % maxFragmentSize != 0);
  missingCount = count;
  read(fragment);
}

bool Reasembler::read(fragment_t fragment) {
  if (fragment.id != id)
    throw "Cannot reasemble fragments of different ids";
  if (fragment.fragmentIndex >= count)
    throw "Received an fragment out of range";
  if (fragment.fragmentIndex != count - 1 &&
      fragment.fragmentSize != maxFragmentSize)
    throw "Intermediate fragment with invalid size";
  if (receivedFragments[fragment.fragmentIndex])
    throw "This fragment has already been received";

  receivedFragments[fragment.fragmentIndex] = true;

  void* dst = ((uint8_t*)data) + (fragment.fragmentIndex * maxFragmentSize);
  memcpy(dst, fragment.data, fragment.fragmentSize);

  return --missingCount == 0;
}

bool Reasembler::isDone() {
  return missingCount == 0;
}

void* Reasembler::getData() {
  return data;
}

uint16_t Reasembler::getTotalSize() {
  return totalSize;
}

Reasembler::~Reasembler() {
  free(data);
}
