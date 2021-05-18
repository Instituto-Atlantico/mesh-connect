#include "listnode.h";

ListNode::ListNode(uint32_t sn, uint32_t sip, uint16_t sp) {
  this->info.sourceNode = sn;
  this->info.sourceIP = sip;
  this->info.sourcePort = sp;
  this->next = NULL;
}

sourceInfo ListNode::getInfo() {
  return this->info;
}

ListNode* ListNode::getNext(){
    return this->next;
}

void ListNode:: setInfo(sourceInfo si){
     this->info = si;
 }

 void ListNode:: setNext(ListNode* ptr){
     this->next = ptr;
 }