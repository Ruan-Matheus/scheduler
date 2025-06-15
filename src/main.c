#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "types.h"
#include "parsing.h"

#define QUANTUM_TEMPO 1
const char* stringsIO[] = { "DISCO", "FITA", "IMPRESSORA" };


int getTempoDeChegada(processo* listaProcessos, int tam, int pos) {
    if (pos >= tam) {
        perror("Erro de index");
        exit(1);
    }

    return listaProcessos[pos].tempoDeChegada;
}


int main(int c, char** argv) {
    FILA baixaPrioridade;
    FILA altaPrioridade;
    FILA IOs;
    int quantum = QUANTUM_TEMPO;
    int tempo = 0;
    processo entradaProcessos[MAX_PROCESSOS];
    char bufferProcessos[BUFFER_SIZE];
    char* arquivoEntrada = "processos_entrada.csv";
    
    FILE* entrada = fopen(arquivoEntrada, "r");
    if (!entrada) {
        perror("fopen");
        exit(1);
    }

    inicializaFila(&altaPrioridade);
    inicializaFila(&baixaPrioridade);
    inicializaFila(&IOs);       

    // Pulando o cabeçalho
    fgets(bufferProcessos, BUFFER_SIZE, entrada);
    fgets(bufferProcessos, BUFFER_SIZE, entrada);
    
    int cont = 0;
    while (cont < MAX_PROCESSOS && fgets(bufferProcessos, BUFFER_SIZE, entrada)) {
        entradaProcessos[cont] = parseProcessos(bufferProcessos);
        cont++;    
    }

    int indice = 0;
    while (indice < cont ||
           !vazioFila(baixaPrioridade) ||
           !vazioFila(altaPrioridade) ||
           !vazioFila(IOs)) {

        
        // Ainda há processo a serem criados
        bool processoCriado = true;
        while (processoCriado) {
            processoCriado = false;
            if (indice < cont) {
                if (tempo >= getTempoDeChegada(entradaProcessos, cont, indice)) {
                    PCB novoProcesso = criandoProcesso(entradaProcessos[indice]);                 
                    printf("PCB criado no instante: %ds\n", tempo);    
                    //enqueue(&altaPrioridade, novoProcesso);
                    indice++;
                    processoCriado = true;
                }
            }
        }


    
        // 2) Verificar retornos de I/O

        // 3) Executar / pré-emptar / bloquear o processo atual

        // Incrementar o tempo
        tempo += quantum;

        if (tempo > 20) {
            break;
        }
    }

    fclose(entrada);
    return 0;
}


