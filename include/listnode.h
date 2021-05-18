#ifndef _LISTNODE_H_
#define _LISTNODE_H_

#include <boolmap.h>
#include <stddef.h>

typedef struct{
  uint32_t sourceNode;
  uint32_t sourceIP;
  uint16_t sourcePort;
}sourceInfo;

class ListNode {
 private:
  sourceInfo info;
  ListNode* next;

 public:
 ListNode(uint32_t sn, uint32_t sip, uint16_t sp);
   
 sourceInfo getInfo();

 ListNode*getNext(){
     return next;
 }

 void setInfo(sourceInfo si){
     this->info = si;
 }

 void setNext(ListNode* ptr){
     this->next = ptr;
 }
};

#endif