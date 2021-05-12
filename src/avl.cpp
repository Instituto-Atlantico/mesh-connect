#include "avl.h"
#include <string.h>
#include "math.h"
#include "nat.h"

AVL::AVL() {
  this->root = NULL;
  this->cntNodes = 0;
}

localRoot* AVL::getTreeRoot() {
  return this->root;
}

uint8_t AVL::getHeight(Node* node) {
  if (node == NULL) {
    return -1;
  }

  return node->height;
}

uint8_t AVL::balanceFactor(Node* node) {
  if (node == NULL) {
    return 0;
  }

  uint8_t calc = MAX(getHeight(node->left), getHeight(node->right));
  return calc;
}

void AVL::rotationLL(localRoot* root) {
  Node* node;
  node = (*root)->left;
  (*root)->left = node->right;
  node->right = (*root);
  (*root)->height =
      MAX(getHeight((*root)->right), getHeight((*root)->left)) + 1;
  node->height = MAX(getHeight(node->left), (*root)->height) + 1;
}

void AVL ::rotationRR(localRoot* root) {
  Node* node;
  node = (*root)->right;
  (*root)->right = node->left;
  node->left = (*root);
  (*root)->height =
      MAX(getHeight((*root)->left), getHeight((*root)->right)) + 1;
  node->height = MAX((*root)->height, getHeight(node->right)) + 1;
  *root = node;
}

void AVL::rotationLR(localRoot* root) {
  rotationRR(&(*root)->left);
  rotationLL(root);
}

void AVL::rotationRL(localRoot* root) {
  rotationLL(&(*root)->right);
  rotationRR(root);
}

bool AVL::equals(gw_nat_flow_entry_t entry1,
                 gw_nat_flow_entry_t entry2,
                 uint8_t type) {
  if (type == 1) {
    if (memcmp(&entry1, &entry2, sizeof(entry1)) == 0) {
      return true;
    } else {
      return false;
    }
  }

  else if (type == 2) {
    if (entry1.destinationPort == entry2.destinationPort &&
        entry1.destinationIP == entry2.destinationIP &&
        entry1.protocol == entry2.protocol) {
      return true;
    } else {
      return false;
    }
  }
}

bool AVL::cmp1(gw_nat_flow_entry_t entry1, gw_nat_flow_entry_t entry2) {
  int8_t value;
  value = memcmp(&entry1, &entry2, sizeof(entry1));
  if (value > 0) {
    return true;
  } else {
    return false;
  }
}

bool AVL::cmp2(gw_nat_flow_entry_t entry1, gw_nat_flow_entry_t entry2) {
  if (entry1.destinationPort > entry2.destinationPort) {
    return true;
  } else if (entry1.destinationPort > entry2.destinationPort) {
    return false;
  }

  else if (entry1.destinationPort == entry2.destinationPort) {
    if (entry1.destinationIP > entry2.destinationIP) {
      return true;
    } else if (entry1.destinationIP < entry2.destinationIP) {
      return false;
    }

    else if (entry1.destinationIP == entry2.destinationIP) {
      if (entry1.protocol > entry2.protocol) {
        return true;
      } else {
        return false;
      }
    }
  }
}
