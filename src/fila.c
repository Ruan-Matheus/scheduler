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


void printFila(FILA f) {
    NO* current = f.INICIO;
    while (current) {
        printf("PID: %d --- Tempo de servico: %d \n", current->chave.PID, current->chave.tempoServico);
        current = current->prox;
    }
    printf("\n");
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


bool dequeue(FILA* f, TIPOCHAVE* chave) {
    if (tamanhoFila(*f) == 0) {
        return false;
    }

    NO* temp = f->INICIO;
    *chave =  temp->chave;
    f->INICIO = f->INICIO->prox;
    
    if (f->INICIO == NULL) {
        f->FIM = NULL;
    }

    free(temp);
    return true;
}


