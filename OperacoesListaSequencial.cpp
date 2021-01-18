#include <iostream>
#include <new>
#include <string>
#include <stdlib.h>

using namespace std; //para utilizar cout

//Struct para pessoa
struct pessoa{
    string nome;
    int      rg;
};

//Limpa a tela
void limpatela(){
    system("CLS");
}

//Imprime a lista Sequencial
void imprimeSequencial(pessoa *ponteiroSequencial,int tamanhoDaListaSequencial) {

   //Cabecaalho da Lista
   cout << "\nLista: \n";

   //Imprime a lista com separacao de virgulas e indices
   int cont;
   for(cont = 0; cont < tamanhoDaListaSequencial ; cont++)
        cout << cont << " - " << ponteiroSequencial[cont].nome << "," << ponteiroSequencial[cont].rg << "\n";

}

//Adiciona um membro ao inicio da lista
void adcComecoSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial,string nome,int rg){

    //Caso a lista nao possua elementos, cria uma nova
    if(*tamanhoDaListaSequencial == 0){

        //Cria uma nova lista
        pessoa *novalistaSequencial = new pessoa[1];

        //Coloca o primeiro membro
        novalistaSequencial[0].nome = nome;
        novalistaSequencial[0].rg   =   rg;

        //Faz o ponteiro apontar para a nova lista
        ponteiroSequencial = novalistaSequencial;

    }else{

        //Cria um vetor auxiliar com uma posicao a mais
        pessoa *copiaListaSequencial = new pessoa[*tamanhoDaListaSequencial + 1];

        //Posiciona o primeiro elemento
        copiaListaSequencial[0].nome = nome;
        copiaListaSequencial[0].rg   =   rg;

        //Passa os elementos da lista antiga para a nova uma posicao a frente
        int cont;
        for(cont = 0; cont < *tamanhoDaListaSequencial; cont++){
            copiaListaSequencial[cont+1].nome = ponteiroSequencial[cont].nome;
            copiaListaSequencial[cont+1].rg   =   ponteiroSequencial[cont].rg;
        }

        //Faz o ponteiroSequencial apontar para a nova lista com um elemento extra
        ponteiroSequencial = copiaListaSequencial;

    }

    //Atualiza o tamanho da lista
    *tamanhoDaListaSequencial = *tamanhoDaListaSequencial + 1;
}

//Adiciona um membro ao fim da lista
void adcFimSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial,string nome,int rg,int *posicaoNaLista){

        //Posicao na lista
        *posicaoNaLista = *tamanhoDaListaSequencial;

        //Cria um vetor com uma posicao a mais
        pessoa *copiaListaSequencial = new pessoa[*tamanhoDaListaSequencial + 1];

        //Posiciona o ultimo elemento no tamanhoNovo -1 , pois come�amos contando do 0
        copiaListaSequencial[*tamanhoDaListaSequencial].nome = nome;
        copiaListaSequencial[*tamanhoDaListaSequencial].rg   =   rg;

        //Passa os elementos da lista antiga para a nova
        int cont;
        for(cont = 0; cont < *tamanhoDaListaSequencial; cont++){
            copiaListaSequencial[cont].nome = ponteiroSequencial[cont].nome;
            copiaListaSequencial[cont].rg   =   ponteiroSequencial[cont].rg;
        }

        //Faz o ponteiroSequencial apontar para a nova lista com um elemento extra
        ponteiroSequencial = copiaListaSequencial;

        //Atualiza o tamanho da lista
        *tamanhoDaListaSequencial = *tamanhoDaListaSequencial + 1;
}

//Adiciona um membro numa posicao especifica
void adcPosicaoSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial,string nome,int rg,int posicao){

        //Cria um vetor com uma posicao a mais
        pessoa *copiaListaSequencial = new pessoa[*tamanhoDaListaSequencial + 1];

        //Passa os elementos da lista antiga para a nova ate o elemento desejado
        int cont;
        for(cont = 0; cont < posicao; cont++){
            copiaListaSequencial[cont].nome = ponteiroSequencial[cont].nome;
            copiaListaSequencial[cont].rg   =   ponteiroSequencial[cont].rg;
        }

        //Posiciona o elemento na posicao desejada
        copiaListaSequencial[posicao].nome = nome;
        copiaListaSequencial[posicao].rg   =   rg;

        //Continua posicionando os outros elementos apos a posicao desejada
        for(cont = posicao + 1; cont < *tamanhoDaListaSequencial + 1; cont++){
            copiaListaSequencial[cont].nome = ponteiroSequencial[cont-1].nome;
            copiaListaSequencial[cont].rg   =   ponteiroSequencial[cont-1].rg;
        }

        //Faz o ponteiroSequencial apontar para a nova lista com um elemento extra
        ponteiroSequencial  = copiaListaSequencial;

        //Atualiza o tamanho da lista
        *tamanhoDaListaSequencial = *tamanhoDaListaSequencial + 1;
}

//Remove o elemento inicial
void removeInicioSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial){

        //Cria um vetor com uma posicao a menos
        pessoa *copiaListaSequencial = new pessoa[*tamanhoDaListaSequencial - 1];

        //Passa os elementos da lista antiga ignorando o primeiro para a nova ate o elemento desejado
        int cont;
        for(cont = 1; cont < *tamanhoDaListaSequencial; cont++){
            copiaListaSequencial[cont-1].nome = ponteiroSequencial[cont].nome;
            copiaListaSequencial[cont-1].rg = ponteiroSequencial[cont].rg;
        }

        //Faz o ponteiroSequencial apontar para a nova lista com um elemento extra
        ponteiroSequencial = copiaListaSequencial;

        //Atualiza o tamanho da lista
        *tamanhoDaListaSequencial = *tamanhoDaListaSequencial - 1;

}

//Remove o elemento final
void removeFimSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial,int *posicaoNaLista){

        //Posicao na lista
        *posicaoNaLista = *tamanhoDaListaSequencial - 1;

        //Cria um vetor com uma posicao a menos
        pessoa *copiaListaSequencial = new pessoa[*tamanhoDaListaSequencial - 1];

        //Passa os elementos da lista antiga ignorando o primeiro para a nova ate o elemento desejado
        int cont;
        for(cont = 0; cont < *tamanhoDaListaSequencial - 1; cont++){
            copiaListaSequencial[cont].nome = ponteiroSequencial[cont].nome;
            copiaListaSequencial[cont].rg   =   ponteiroSequencial[cont].rg;
        }

        //Faz o ponteiroSequencial apontar para a nova lista com um elemento extra
        ponteiroSequencial = copiaListaSequencial;

        //Atualiza o tamanho da lista
        *tamanhoDaListaSequencial = *tamanhoDaListaSequencial - 1;

}

//Remove um elemento por posicao
void removePosicaoSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial,int posicao){

        //Cria um vetor com uma posicao a mais
        pessoa *copiaListaSequencial = new pessoa[*tamanhoDaListaSequencial - 1];

        //Passa os elementos da lista antiga para a nova ate o elemento desejado
        int cont;
        for(cont = 0; cont < *tamanhoDaListaSequencial - 1; cont++){

            if(cont < posicao){
                //Faz uma copia dos elementos ate a posicao
                copiaListaSequencial[cont].nome = ponteiroSequencial[cont].nome;
                copiaListaSequencial[cont].rg   =   ponteiroSequencial[cont].rg;
            }else{
                //Faz uma copia dos elementos ate a posicao
                copiaListaSequencial[cont].nome = ponteiroSequencial[cont+1].nome;
                copiaListaSequencial[cont].rg   =   ponteiroSequencial[cont+1].rg;
            }

        }

        //Faz o ponteiroSequencial apontar para a nova lista com um elemento extra
        ponteiroSequencial = copiaListaSequencial;

        //Atualiza o tamanho da lista
        *tamanhoDaListaSequencial = *tamanhoDaListaSequencial - 1;
}

//Retorna o nome pelo RG
string retornaNomeSequencial(pessoa *&ponteiroSequencial,int *tamanhoDaListaSequencial,int rg,int *posicao){

        //Nome a ser retornado
        string nome = "Nao Encontrado";

        //Passa os elementos da lista antiga para a nova ate o elemento desejado
        int cont;
        for(cont = 0; cont < *tamanhoDaListaSequencial; cont++){

            //Se encontrar alguem com o RG procurado
            if(ponteiroSequencial[cont].rg == rg){

                //Faz uma copia dos elementos ate a posicao
                nome = ponteiroSequencial[cont].nome;

                cout << "Encontrado na Posicao:" << cont <<"\n";

            }
        }

        return nome;

}

int main()
{

    //Variaveis
    int funcaoDesejada = 1;

    //Lista Sequencial Inicial com tamanho 1 e vazia
    //pessoa *copiaListaSequencial = new pessoa[1];

    //Dois registros de exemplo
    /*
    copiaListaSequencial[0].nome = "John";
    copiaListaSequencial[0].rg = 123;

    copiaListaSequencial[1].nome = "Maicon";
    copiaListaSequencial[1].rg = 123;
    */

    //Lista Sequencial Inicial
    pessoa *ponteiroSequencial;

    //Aponta para a lista inicial vazia
    //ponteiroSequencial = copiaListaSequencial;

    //Tamanho inicial da lista
    int tamanhoDaListaSequencial = 0;

     //Manipula as listas
    while(funcaoDesejada < 10 && funcaoDesejada > 0){

        cout << "Tamanho Atual[" << tamanhoDaListaSequencial << "]\n";
        cout << "Operacoes \n";
        cout << "1 - Insercao de um node no inicio da lista \n";
        cout << "2 - Insercao de um node no fim da lista \n";
        cout << "3 - Insercao de um node na posicao N \n";
        cout << "4 - Retirar um node do inicio da lista \n";
        cout << "5 - Retirar um node no fim da lista \n";
        cout << "6 - Retirar um node na posicao N \n";
        cout << "7 - Procurar um node com o campo RG \n";
        cout << "8 - Imprimir a Lista. \n";
        cout << "9 - Sair do sistema. \n";
        cout << "\nEscolha um numero e pressione ENTER: \n";

        //Le a opcao
        cin >> funcaoDesejada;

        //Limpa as opces
        limpatela();

        //Variaveis para valores novos
        string nome;
        int rg, posicao,posicaoNaLista;

        switch(funcaoDesejada){
            case 1:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Inserir um node no comeco da lista";

                //Lendo valores do usuario
                cout << "\nDigite o nome: ";
                cin >> nome;
                cout << "Digite o RG: ";
                cin >> rg;

                //Adiciona ao inicio da Lista ou cria uma nova
                adcComecoSequencial(ponteiroSequencial,&tamanhoDaListaSequencial, nome, rg);

                //Exibe o usuario inserido
                cout << "\nUsuario: " << nome << ",RG: " << rg << " adicionado ao inicio da lista.\n";

                break;

            case 2:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Inserir um node no fim da lista";

                //Lendo valores do usuario
                cout << "\nDigite o nome: ";
                cin >> nome;
                cout << "Digite o RG: ";
                cin >> rg;

                if(tamanhoDaListaSequencial == 0){
                    //Se for o primeiro da lista cria uma nova lista
                    adcComecoSequencial(ponteiroSequencial,&tamanhoDaListaSequencial, nome, rg);
                }else{
                    //Adiciona ao fim da Lista
                    adcFimSequencial(ponteiroSequencial,&tamanhoDaListaSequencial, nome, rg, &posicaoNaLista);
                }

                //Exibe o usuario inserido
                cout << "Usuario: " << nome << ",RG: " << rg << " adicionado ao fim da lista(" << posicaoNaLista << ")";

                break;

             case 3:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Inserir um node na posicao N da lista";

                 //Lendo valores do usuario
                cout << "\nDigite a Posicao: ";
                cin >> posicao;
                cout << "\nDigite o nome: ";
                cin >> nome;
                cout << "Digite o RG: ";
                cin >> rg;

                //Se pedir a primeira posicao
                if(posicao == 0){
                    //Se for o primeiro da lista cria uma nova lista
                    adcComecoSequencial(ponteiroSequencial,&tamanhoDaListaSequencial, nome, rg);
                }else if(posicao == tamanhoDaListaSequencial + 1){
                    //Adiciona ao fim da Lista
                    adcFimSequencial(ponteiroSequencial,&tamanhoDaListaSequencial, nome, rg, &posicaoNaLista);
                }else{
                    //Adiciona numa posicao especifica
                    adcPosicaoSequencial(ponteiroSequencial, &tamanhoDaListaSequencial, nome, rg, posicao);
                }

                //Exibe o usuario inserido
                cout << "Usuario: " << nome << ",RG: " << rg << " adicionado na posicao" << posicao<< "da lista.";

                break;

            case 4:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Retirar um node do inicio da lista\n";

                if(tamanhoDaListaSequencial == 0){
                    cout << "Lista Vazia\n";
                }else{
                    //Remove do inicio da lista
                    removeInicioSequencial(ponteiroSequencial, &tamanhoDaListaSequencial);
                    //Exibe o usuario removido
                    cout << "Usuario: " << nome <<",RG: "<<rg<< "foi removido do inicio da lista";
                }

                break;

            case 5:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Retirar um node do fim da lista\n";

                if(tamanhoDaListaSequencial == 0){
                    cout << "Lista Vazia\n";
                }else{
                    //Remove do final da lista
                    removeFimSequencial(ponteiroSequencial, &tamanhoDaListaSequencial, &posicaoNaLista);
                    //Exibe o usuario removido
                    cout << "Usuario: "<<nome<<",RG: "<<rg<<" foi removido da posicao " << posicaoNaLista <<" da lista.";
                }

                break;

            case 6:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Retirar um node do de uma posicao da lista\n";

                 //Lendo valores do usuario
                cout << "Digite a Posicao: ";
                cin >> posicao;

                if(tamanhoDaListaSequencial == 0){
                    cout << "Lista Vazia\n";
                }else{

                    if(posicao == 0){
                        //Remove do inicio da lista
                        removeInicioSequencial(ponteiroSequencial, &tamanhoDaListaSequencial);
                    }else if(posicao == tamanhoDaListaSequencial - 1){
                        //Remove do final da lista
                        removeFimSequencial(ponteiroSequencial, &tamanhoDaListaSequencial, &posicaoNaLista);
                    }else{
                        //Remove do posicao desejada da lista
                        removePosicaoSequencial(ponteiroSequencial,&tamanhoDaListaSequencial,posicao);
                    }

                }

                //Exibe o usuario removido
                cout << "Usuario: "<<nome<<",RG: "<<rg<<" foi removido da posicao "<< posicao <<" da lista.";

                break;

            case 7:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Retorna o nome e posicao pelo RG \n";

                //Lendo valores do usuario
                cout << "Digite um RG: ";
                cin >> rg;

                //Retorna o nome pelo RG
                cout << "Nome:" << retornaNomeSequencial(ponteiroSequencial, &tamanhoDaListaSequencial, rg, &posicao);

                break;

            case 8:

                //Cabecalho da acao
                cout << "Funcao Escolhida: Imprime a lista\n";

                imprimeSequencial(ponteiroSequencial, tamanhoDaListaSequencial);

                break;
        }
    }

    return 0;
}
