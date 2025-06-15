#ifndef FILA_H
#define FILA_H
#include "types.h"

typedef struct node {
    TIPOCHAVE chave;
    struct node* prox;
} NO;


typedef struct {
    NO* INICIO;
    NO* FIM;   
} FILA;

void inicializaFila(FILA* f);
void printFila(FILA f);
int tamanhoFila(FILA f);
bool vazioFila(FILA f);
bool enqueue(FILA *f, TIPOCHAVE chave);
TIPOCHAVE* dequeue(FILA *f);


#endif
