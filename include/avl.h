#ifndef _AVL_H_
#define _AVL_H_
#define MAX(X,Y) ((X)>(Y)?(X):(Y))
#define MIN(X,Y) ((X)<(Y)?(X):(Y))
#define ABS(X) ((X)<0?(X*(-1)):(X))

#include <boolmap.h>
#include <stddef.h>
#include "message.h"
#include "nat.h"
typedef struct Node* localRoot;

struct dest{
  uint8_t protocol;
  uint32_t destinationIP;
  uint16_t destinationPort;
};

struct source{
  uint32_t sourceNode;
  uint32_t sourceIP;
  uint16_t sourcePort;
};

struct Node{
  gw_nat_flow_entry_t dest;
  source linkedlist;
  uint8_t height;
  uint8_t index;
  struct Node* left;
  struct Node* right;
};


class AVL {
 private:
  localRoot* root;
  uint8_t cntNodes;

 public:
  AVL();
  localRoot *getTreeRoot();
  uint8_t getHeight(Node *node);
  uint8_t balanceFactor(Node *node);
  void rotationLL(localRoot *root);
  void rotationRR(localRoot *root);
  void rotationLR(localRoot *root);
  void rotationRL(localRoot *root);
  bool equals(gw_nat_flow_entry_t entry1, gw_nat_flow_entry_t entry2, uint8_t type);
  bool cmp1(gw_nat_flow_entry_t entry1, gw_nat_flow_entry_t entry2);
  bool cmp2(gw_nat_flow_entry_t entry1, gw_nat_flow_entry_t entry2);
  bool insert(uint8_t index, gw_nat_flow_entry_t entry);
  bool search(gw_nat_flow_entry_t entry);
};

#endif
