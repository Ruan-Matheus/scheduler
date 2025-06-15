#include <stdio.h>
#include <stdlib.h>
#include "fila.h"


void inicializaFila(FILA* f) {
    f->INICIO = NULL;
    f->FIM = NULL;
}


int tamanhoFila(FILA f) {
    int cont = 0;

    NO* current = f.INICIO;
    while (current) {
        cont++;
        current = current->prox;
    }
    return cont;
}

bool vazioFila(FILA f) {
    if (f.INICIO == NULL) {
        return true;
    }
    return false;
}


// Check how to do this
void printFila(FILA f) {
    NO* current = f.INICIO;
    while (current) {
        //printf("%d ", current->chave);
        current = current->prox;
    }
}


bool enqueue(FILA *f, TIPOCHAVE chave) {
    NO* novoNo = malloc(sizeof(NO));
    if (novoNo == NULL) {
        printf("Erro alocando memoria!");
        return false;
    }
    novoNo->chave = chave;
    novoNo->prox = NULL;

    if (f->INICIO == NULL) {
        f->INICIO = novoNo;
    }

    else {
        f->FIM->prox = novoNo;
    }

    f->FIM = novoNo;
    return true;
}


TIPOCHAVE* dequeue(FILA *f) {
    if (tamanhoFila(*f) == 0) {
        return NULL;
    }

    TIPOCHAVE* topo = &f->INICIO->chave;
    NO* clean = f->INICIO;
    f->INICIO = f->INICIO->prox;
    free(clean);
    return topo;
}


