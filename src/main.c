#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "types.h"
#include "parsing.h"

#define QUANTUM_TEMPO 1
const char* stringsIO[] = { "DISCO", "FITA", "IMPRESSORA" };


int getTempoDeChegada(ProcessDescriptor* listaProcessos, int tam, int pos) {
    if (pos >= tam) {
        perror("Erro de index");
        exit(1);
    }

    return listaProcessos[pos].tempoDeChegada;
}

PCB* getProximoProcesso(FILA* altaPrioridade, FILA* baixaPrioridade) {
    if (!vazioFila(*altaPrioridade)) {
        return dequeue(altaPrioridade);
    }

    if (!vazioFila(*baixaPrioridade)) {
        return dequeue(baixaPrioridade);
    }

    return NULL;
}


int main(int c, char** argv) {
    int tempo = 0;
    int quantum = QUANTUM_TEMPO;
    char bufferProcessos[BUFFER_SIZE];
    char* arquivoEntrada = "processos_entrada.csv";
    FILA baixaPrioridade;
    FILA altaPrioridade;
    FILA IOs;
    ProcessDescriptor entradaProcessos[MAX_PROCESSOS];
    PCB* processoEmExecucao = NULL;
    
    FILE* entrada = fopen(arquivoEntrada, "r");
    if (!entrada) {
        perror("fopen");
        exit(1);
    }

    inicializaFila(&altaPrioridade);
    inicializaFila(&baixaPrioridade);
    inicializaFila(&IOs);       
    processoEmExecucao = malloc(sizeof(PCB));

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
        // Checando o processo que sofreu preempção / finalizou
        // Checar se precisa ir para IO aqui
        if (processoEmExecucao) {
            processoEmExecucao->status = PRONTO;
            enqueue(&baixaPrioridade, *processoEmExecucao);
        }
        
        processoEmExecucao = getProximoProcesso(&baixaPrioridade, &altaPrioridade);
        if (processoEmExecucao) {
            processoEmExecucao->status = EXECUTANDO;
            processoEmExecucao->tempoServico -= quantum;
        }

        // Incrementar o tempo
        tempo += quantum;

        if (tempo > 20) {
            break;
        }
    }

    fclose(entrada);
    return 0;
}


