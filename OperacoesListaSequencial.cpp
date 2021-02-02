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
void display(node* operationsPointer, int sizeNode) {
  // Cabecaalho da Lista
  Serial.print("\nNode: \n");

  // Imprime a lista com separacao de vinumberulas e indices
  int positionCounterVector;
  for (positionCounterVector = 0; positionCounterVector < sizeNode;
       positionCounterVector++)
    Serial.print(positionCounterVector);
  Serial.print(" - ");
  Serial.print(operationsPointer[positionCounterVector].name);
  Serial.print(", ");
  Serial.println(operationsPointer[positionCounterVector].number);
}

// Adiciona um membro ao inicio da lista
void insertFront(node*& operationsPointer,
                 int* sizeNode,
                 std::string name,
                 int number) {
  // Caso a lista nao possua elementos, cria uma nova
  if (*sizeNode == 0) {
    // Cria uma nova lista
    node* newNode = new node;

    // Coloca o primeiro membro
    newNode[0].name = name;
    newNode[0].number = number;

    // Faz o ponteiro apontar para a nova lista
    operationsPointer = newNode;
  } else {
    // Cria um vetor auxiliar com uma position a mais
    node* copyList = new node[*sizeNode + 1];

    // Posiciona o primeiro elemento
    copyList[0].name = name;
    copyList[0].number = number;

    // Passa os elementos da lista antiga para a nova uma position a frente
    int positionCounterVector;
    for (positionCounterVector = 0; positionCounterVector < *sizeNode;
         positionCounterVector++) {
      copyList[positionCounterVector + 1].name =
          operationsPointer[positionCounterVector].name;
      copyList[positionCounterVector + 1].number =
          operationsPointer[positionCounterVector].number;
    }

    // Faz o operationsPointer apontar para a nova lista com um elemento extra
    operationsPointer = copyList;
  }

  // Atualiza o tamanho da lista
  *sizeNode = *sizeNode + 1;
}

// Adiciona um membro ao fim da lista
void insertBehind(node*& operationsPointer,
                  int* sizeNode,
                  std::string name,
                  int number,
                  int* positionNode) {
  // position na lista
  *positionNode = *sizeNode;

  // Cria um vetor com uma position a mais
  node* copyList = new node[*sizeNode + 1];

  // Posiciona o ultimo elemento no tamanhoNovo -1 , pois comecamos
  // positionCounterVector ando do
  // 0
  copyList[*sizeNode].name = name;
  copyList[*sizeNode].number = number;

  // Passa os elementos da lista antiga para a nova
  int positionCounterVector;
  for (positionCounterVector = 0; positionCounterVector < *sizeNode;
       positionCounterVector++) {
    copyList[positionCounterVector].name =
        operationsPointer[positionCounterVector].name;
    copyList[positionCounterVector].number =
        operationsPointer[positionCounterVector].number;
  }

  // Faz o operationsPointer apontar para a nova lista com um elemento extra
  operationsPointer = copyList;
  // Atualiza o tamanho da lista
  *sizeNode = *sizeNode + 1;
}

// Adiciona um membro numa position especifica
void insertNodePosition(node*& operationsPointer,
                        int* sizeNode,
                        std::string name,
                        int number,
                        int position) {
  // Cria um vetor com uma position a mais
  node* copyList = new node[*sizeNode + 1];

  // Passa os elementos da lista antiga para a nova ate o elemento desejado
  int positionCounterVector;
  for (positionCounterVector = 0; positionCounterVector < position;
       positionCounterVector++) {
    copyList[positionCounterVector].name =
        operationsPointer[positionCounterVector].name;
    copyList[positionCounterVector].number =
        operationsPointer[positionCounterVector].number;
  }

  // Posiciona o elemento na position desejada
  copyList[position].name = name;
  copyList[position].number = number;

  // positionCounterVector inua posicionando os outros elementos apos a position
  // desejada
  for (positionCounterVector = position + 1;
       positionCounterVector < *sizeNode + 1; positionCounterVector++) {
    copyList[positionCounterVector].name =
        operationsPointer[positionCounterVector - 1].name;
    copyList[positionCounterVector].number =
        operationsPointer[positionCounterVector - 1].number;
  }

  // Faz o operationsPointer apontar para a nova lista com um elemento extra
  operationsPointer = copyList;

  // Atualiza o tamanho da lista
  *sizeNode = *sizeNode + 1;
}

// Remove o elemento inicial
void deleteNodeFront(node*& operationsPointer, int* sizeNode) {
  // Cria um vetor com uma position a menos
  node* copyList = new node[*sizeNode - 1];

  // Passa os elementos da lista antiga ignorando o primeiro para a nova ate o
  // elemento desejado
  int positionCounterVector;
  for (positionCounterVector = 1; positionCounterVector < *sizeNode;
       positionCounterVector++) {
    copyList[positionCounterVector - 1].name =
        operationsPointer[positionCounterVector].name;
    copyList[positionCounterVector - 1].number =
        operationsPointer[positionCounterVector].number;
  }

  // Faz o operationsPointer apontar para a nova lista com um elemento extra
  operationsPointer = copyList;

  // Atualiza o tamanho da lista
  *sizeNode = *sizeNode - 1;
}

// Remove o elemento final
void deleteNodeBehind(node*& operationsPointer,
                      int* sizeNode,
                      int* positionNode) {
  // position na lista
  *positionNode = *sizeNode - 1;

  // Cria um vetor com uma position a menos
  node* copyList = new node[*sizeNode - 1];

  // Passa os elementos da lista antiga ignorando o primeiro para a nova ate o
  // elemento desejado
  int positionCounterVector;
  for (positionCounterVector = 0; positionCounterVector < *sizeNode - 1;
       positionCounterVector++) {
    copyList[positionCounterVector].name =
        operationsPointer[positionCounterVector].name;
    copyList[positionCounterVector].number =
        operationsPointer[positionCounterVector].number;
  }

  // Faz o operationsPointer apontar para a nova lista com um elemento extra
  operationsPointer = copyList;

  // Atualiza o tamanho da lista
  *sizeNode = *sizeNode - 1;
}

// Remove um elemento por position
void deleteNodePosition(node*& operationsPointer, int* sizeNode, int position) {
  // Cria um vetor com uma position a mais
  node* copyList = new node[*sizeNode - 1];

  // Passa os elementos da lista antiga para a nova ate o elemento desejado
  int positionCounterVector;
  for (positionCounterVector = 0; positionCounterVector < *sizeNode - 1;
       positionCounterVector++) {
    if (positionCounterVector < position) {
      // Faz uma copia dos elementos ate a position
      copyList[positionCounterVector].name =
          operationsPointer[positionCounterVector].name;
      copyList[positionCounterVector].number =
          operationsPointer[positionCounterVector].number;
    } else {
      // Faz uma copia dos elementos ate a position
      copyList[positionCounterVector].name =
          operationsPointer[positionCounterVector + 1].name;
      copyList[positionCounterVector].number =
          operationsPointer[positionCounterVector + 1].number;
    }
  }

  // Faz o operationsPointer apontar para a nova lista com um elemento extra
  operationsPointer = copyList;
  // Atualiza o tamanho da lista
  *sizeNode = *sizeNode - 1;
}

// Retorna o name pelo number
std::string printNode(node*& operationsPointer,
                      int* sizeNode,
                      int number,
                      int* position) {
  // name a ser retornado
  std::string name = "The node was not found!";

  // Passa os elementos da lista antiga para a nova ate o elemento desejado
  int positionCounterVector;
  for (positionCounterVector = 0; positionCounterVector < *sizeNode;
       positionCounterVector++) {
    // Se enpositionCounterVector rar alguem com o number procurado
    if (operationsPointer[positionCounterVector].number == number) {
      // Faz uma copia dos elementos ate a position
      name = operationsPointer[positionCounterVector].name;
      Serial.print("Found in position: ");
      Serial.println(positionCounterVector);
    }
  }

  return name;
}
