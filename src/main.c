#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "types.h"
#include "parsing.h"
#define VERMELHO "\033[0;31m"
#define VERDE "\033[0;32m"
#define AMARELO "\033[0;33m"
#define AZUL "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define RESET "\033[0m"

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


void criandoProcessosParaChegada(int *contProcessCriados, int contProcessLidos, int tempo, ProcessDescriptor* listaEntradaProcessos, FILA* altaPrioridade) {
    bool processoCriado = true;
    while (processoCriado) {
        processoCriado = false;
        if ((*contProcessCriados) < contProcessLidos) {
            if (tempo >= getTempoDeChegada(listaEntradaProcessos, contProcessLidos, *contProcessCriados)) {
                PCB novoProcesso = criandoProcesso(listaEntradaProcessos[*contProcessCriados]);                 
                printf(AZUL"Processo p%d criado no instante: %d\n"RESET, novoProcesso.PID, tempo);    
                enqueue(altaPrioridade, novoProcesso);
                (*contProcessCriados)++;
                processoCriado = true;
            }
        }
    }
}


void retornaProcessosIOs(FILA* IOs, FILA* altaPrioridade, int tempo) {
    int tam = tamanhoFila(*IOs);
    for (int i = 0; i < tam; i++) {
        PCB p;
        dequeue(IOs, &p);
        if (p.tempoDeRetornoIO == tempo) {
            p.status = PRONTO;
            enqueue(altaPrioridade, p);
            printf(MAGENTA"Processo p%d saiu do IO no instante: %d\n"RESET,
                    p.PID, tempo);
        }
        else {
            // ainda não chegou a hora de retorno
            enqueue(IOs, p);
        }
    }
}


void preemptarProcesso(PCB* processo, FILA* baixaPrioridade) {
    if (processo) {
        processo->status = PRONTO;
        enqueue(baixaPrioridade, *processo);
    }
}


void finalizarProcesso(PCB** processo, int tempo) {
    if (*processo && (*processo)->tempoServico <= 0) {
        printf(VERDE "Processo p%d terminado no instante: %d\n" RESET , (*processo)->PID, tempo);
        free(*processo);
        *processo = NULL;
    }
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


        // Processo finalizado
        finalizarProcesso(&processoEmExecucao, tempo);
        
        // 1) Lendo novos processos
        criandoProcessosParaChegada(&indice, cont, tempo,  entradaProcessos, &altaPrioridade);

        // 2) Verificar retornos de I/O 
        retornaProcessosIOs(&IOs, &altaPrioridade, tempo);

        // 3) Retornar processo executado para a fila de baixa prioridade - Preempcao
        preemptarProcesso(processoEmExecucao, &baixaPrioridade);


        // Executando o proximo processo
        processoEmExecucao = getProximoProcesso(&altaPrioridade, &baixaPrioridade);
        if (!processoEmExecucao) {
            tempo += quantum;
            continue;
        }
        
        processoEmExecucao->status = EXECUTANDO;
        processoEmExecucao->tempoServico -= quantum;
        processoEmExecucao->tempoCpuAcumulado += quantum;
        printf(AMARELO"Processo p%d executado no instante: %d --- Tempo de servico: %d\n"RESET, processoEmExecucao->PID, tempo, processoEmExecucao->tempoServico);

        // Incrementar o tempo
        tempo += quantum;
        
        
        // Processo bloqueado 
        if ((processoEmExecucao->contIOs > processoEmExecucao->proxIO)
           && (processoEmExecucao->tempoCpuAcumulado == processoEmExecucao->temposIOs[processoEmExecucao->proxIO])) {
            processoEmExecucao->status = BLOQUEADO;
            processoEmExecucao->tempoDeRetornoIO = temposParaTiposIO[processoEmExecucao->proxIO] + tempo;
            processoEmExecucao->proxIO += 1;
            enqueue(&IOs, *processoEmExecucao);
            printf(VERMELHO"Processo p%d bloqueado para IO no instante: %d --- Tempo de retorno: %d\n"RESET, processoEmExecucao->PID, tempo, processoEmExecucao->tempoDeRetornoIO);
            processoEmExecucao = NULL;
        }
    }

    fclose(entrada);
    return 0;
}


