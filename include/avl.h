#ifndef _AVL_H_
#define _AVL_H_

#include <boolmap.h>
#include <stddef.h>
#include "message.h"

struct tree_node {
  bool freeEntry : 1;
  uint8_t protocol : 7;
  uint32_t sourceNode;
  uint32_t sourceIP;
  uint16_t sourcePort;
  uint32_t destinationIP;
  uint16_t destinationPort;
  uint8_t height;
  struct tree_node* left;
  struct tree_node* right;
};

class AVL {
 private:
  tree_node* root;

 public:
  AVL();
  uint8_t calcHeight(struct tree_node* node);
  uint8_t balancedFactor(struct tree_node* node);
  bool compare(struct tree_node* node1, struct tree_node* node2);
  void rotationLL();
  void rotationRR();
  void rotationLR();
  void rotationRL();
  void insert(tree_node* node);
  void deleteNode(tree_node* node);
  bool findNode(tree_node* node);
};

#endif
