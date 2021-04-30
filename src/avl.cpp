#include "avl.h"
#include "math.h"
#include "nat.h"

AVL::AVL() {
  this->root = NULL;
}

uint8_t AVL::calcHeight(struct tree_node* node) {
  if (node == NULL) {
    return -1;
  }

  return node->height;
}

uint8_t AVL::balancedFactor(struct tree_node* node) {
  return abs(calcHeight(node->left) - calcHeight(node->right));
}