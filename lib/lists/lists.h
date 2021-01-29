#include <Arduino.h>
#include <stdlib.h>
#include <iostream>
#include <new>
#include <string>

// Struct para node
struct node {
  std::string name;
  int number;
};

// Imprime a lista Sequencial
void display(node* operationsPointer, int sizeNode);

// Adiciona um membro ao inicio da lista
void insertFront(node*& operationsPointer,
                 int* sizeNode,
                 std::string name,
                 int number);

// Adiciona um membro ao fim da lista
void insertBehind(node*& operationsPointer,
                  int* sizeNode,
                  std::string name,
                  int number,
                  int* positionNode);

// Adiciona um membro numa position especifica
void insertNodePosition(node*& operationsPointer,
                        int* sizeNode,
                        std::string name,
                        int number,
                        int position);

// Remove o elemento inicial
void deleteNodeFront(node*& operationsPointer, int* sizeNode);

// Remove o elemento final
void deleteNodeBehind(node*& operationsPointer,
                      int* sizeNode,
                      int* positionNode);

// Remove um elemento por position
void deleteNodePosition(node*& operationsPointer, int* sizeNode, int position);

// Retorna o name pelo number
std::string printNode(node*& operationsPointer,
                      int* sizeNode,
                      int number,
                      int* position);
