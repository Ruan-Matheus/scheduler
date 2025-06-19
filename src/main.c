#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "types.h"
#include "parsing.h"

#define QUANTUM_TEMPO 1
const char* stringsIO[] = {"DISCO", "FITA", "IMPRESSORA"};
const int temposParaTiposIO[] = {3, 5, 2};


int getTempoDeChegada(ProcessDescriptor* listaProcessos, int tam, int pos) {
    if (pos >= tam) {
        perror("Erro de index");
        exit(1);
    }

    return listaProcessos[pos].tempoDeChegada;
}

PCB* getProximoProcesso(FILA* altaPrioridade, FILA* baixaPrioridade) {
    PCB* processo = malloc(sizeof(PCB));
    if (!processo) {
        perror("Erro alocando memoria para o processo");
        exit(1);
    }

    if (dequeue(altaPrioridade, processo)) {
        return processo;
    }

    if (dequeue(baixaPrioridade, processo)) {
        return processo;
    }

    free(processo);
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

    // Pulando o cabeçalho
    fgets(bufferProcessos, BUFFER_SIZE, entrada);
    fgets(bufferProcessos, BUFFER_SIZE, entrada);
    

    // Lendo os processos de entrada
    int cont = 0;
    while (cont < MAX_PROCESSOS && fgets(bufferProcessos, BUFFER_SIZE, entrada)) {
        entradaProcessos[cont] = parseProcessos(bufferProcessos);
        cont++;    
    }


    // Loop principal
    int indice = 0;
    while (processoEmExecucao ||
           indice < cont ||
           !vazioFila(baixaPrioridade) ||
           !vazioFila(altaPrioridade) ||
           !vazioFila(IOs)) {

        
        // 1) Ainda há processo a serem criados
        bool processoCriado = true;
        while (processoCriado) {
            processoCriado = false;
            if (indice < cont) {
                if (tempo >= getTempoDeChegada(entradaProcessos, cont, indice)) {
                    PCB novoProcesso = criandoProcesso(entradaProcessos[indice]);                 
                    printf("Processo p%d criado no instante: %d\n", novoProcesso.PID, tempo);    
                    enqueue(&altaPrioridade, novoProcesso);
                    indice++;
                    processoCriado = true;
                }
            }
        }

        // Processo finalizado
        if (processoEmExecucao && processoEmExecucao->tempoServico <= 0) {
            printf("Processo p%d terminado no instante: %d\n", processoEmExecucao->PID, tempo);
            free(processoEmExecucao);
            processoEmExecucao = NULL; // Adicionar algo mais? Aqui que eu devo dar free no no de PCB?
        }


        // 2) Verificar retornos de I/O 
        int n = tamanhoFila(IOs);
        for (int i = 0; i < n; i++) {
            PCB p;
            dequeue(&IOs, &p);
            if (p.tempoDeRetornoIO == tempo) {
                p.status = PRONTO;
                enqueue(&altaPrioridade, p);
                printf("Processo p%d saiu do IO no instante: %d\n",
                    p.PID, tempo);
            }
            else {
                // ainda não chegou a hora de retorno
                enqueue(&IOs, p);
            }
        }

        // 3) Retornar processo executado para a fila de baixa prioridade - Preempcao
        if (processoEmExecucao) {
            processoEmExecucao->status = PRONTO;
            enqueue(&baixaPrioridade, *processoEmExecucao);
        }


        // Executando o proximo processo
        processoEmExecucao = getProximoProcesso(&altaPrioridade, &baixaPrioridade);
        if (!processoEmExecucao) {
            tempo += quantum;
            continue;
        }
        
        processoEmExecucao->status = EXECUTANDO;
        processoEmExecucao->tempoServico -= quantum;
        processoEmExecucao->tempoCpuAcumulado += quantum;
        printf("Processo p%d executado no instante: %d --- Tempo de servico: %d\n", processoEmExecucao->PID, tempo, processoEmExecucao->tempoServico);

        // Incrementar o tempo
        tempo += quantum;
        
        
        // Processo bloqueado 
        if ((processoEmExecucao->contIOs > processoEmExecucao->proxIO)
           && (processoEmExecucao->tempoCpuAcumulado == processoEmExecucao->temposIOs[processoEmExecucao->proxIO])) {
            processoEmExecucao->status = BLOQUEADO;
            processoEmExecucao->tempoDeRetornoIO = temposParaTiposIO[processoEmExecucao->proxIO] + tempo;
            processoEmExecucao->proxIO += 1;
            enqueue(&IOs, *processoEmExecucao);
            printf("Processo p%d bloqueado no instante: %d --- Tempo de retorno: %d\n", processoEmExecucao->PID, tempo, processoEmExecucao->tempoDeRetornoIO);
            processoEmExecucao = NULL;
        }
    }

    fclose(entrada);
    return 0;
}


