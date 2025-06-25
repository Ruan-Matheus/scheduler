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

#define QUANTUM_TEMPO 5
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
                printf(AZUL"[T:%03d] CRIADO     | P%d chegou ao sistema\n"RESET, tempo, novoProcesso.PID);    
                enqueue(altaPrioridade, novoProcesso);
                (*contProcessCriados)++;
                processoCriado = true;
            }
        }
    }
}


void retornaProcessosIOs(FILA* IOs, FILA* altaPrioridade, FILA* baixaPrioridade, int tempo) {
    int tam = tamanhoFila(*IOs);
    for (int i = 0; i < tam; i++) {
        PCB p;
        dequeue(IOs, &p);
        if (p.tempoDeRetornoIO == tempo) {
            p.status = PRONTO;

            int indiceIOExecutado = p.proxIO - 1;
            
            // Lógica de decisão (já estava correta)
            if (indiceIOExecutado >= 0 && p.tiposIOs[indiceIOExecutado] == DISCO) {
                enqueue(baixaPrioridade, p);
            } else {
                enqueue(altaPrioridade, p);
            }

            // CORREÇÃO: Usa o tipo de IO correto para imprimir a mensagem
            IO tipoIOFinalizado = p.tiposIOs[indiceIOExecutado];
            printf(MAGENTA"[T:%03d] DESBLOQ    | P%d retornou do I/O [%s]\n"RESET, tempo, p.PID, stringsIO[tipoIOFinalizado]);
        }
        else {
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


void finalizarProcesso(PCB* processo, int tempo) {
    printf(VERDE"[T:%03d] FINALIZ    | P%d terminou execucao\n"RESET, tempo, processo->PID);
    free(processo);
}


void bloquearProcesso(PCB* processo, FILA* IOS, int tempo) {
    processo->status = BLOQUEADO;
    
    // Pega o tipo de IO correto que será executado
    IO tipoDoIOAtual = processo->tiposIOs[processo->proxIO];
    
    // CORREÇÃO: Usa a variável 'tipoDoIOAtual' para obter o tempo e o nome
    processo->tempoDeRetornoIO = temposParaTiposIO[tipoDoIOAtual] + tempo;
    
    printf(VERMELHO"[T:%03d] BLOQUEIO   | P%d iniciou I/O [%s] (retorna em T:%03d)\n"RESET, 
        tempo, processo->PID, stringsIO[tipoDoIOAtual], processo->tempoDeRetornoIO);

    processo->proxIO += 1;
    enqueue(IOS, *processo);
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

    // Cabeçalho da simulação
    printf("\n");
    printf("===============================================================\n");
    printf("                    SIMULACAO DO ESCALONADOR                   \n");
    printf("===============================================================\n");
    printf("TEMPO  | EVENTO     | DESCRICAO\n");
    printf("===============================================================\n");

    // Loop principal
    int indice = 0;
    while (processoEmExecucao ||
        indice < cont ||
        !vazioFila(baixaPrioridade) ||
        !vazioFila(altaPrioridade) ||
        !vazioFila(IOs)) {

        // 1) Lendo novos processos
        criandoProcessosParaChegada(&indice, cont, tempo,  entradaProcessos, &altaPrioridade);

        // 2) Verificar retornos de I/O 
        retornaProcessosIOs(&IOs, &altaPrioridade, &baixaPrioridade, tempo);

        // Executando o proximo processo
        if (!processoEmExecucao) {
            processoEmExecucao = getProximoProcesso(&altaPrioridade, &baixaPrioridade);
            if (!processoEmExecucao) {
                tempo++;
                continue;
            }
        }
        

        printf(AMARELO"[T:%03d] EXEC       | P%d iniciou quantum (%d ticks)\n"RESET, 
            tempo, processoEmExecucao->PID, quantum);
        
        for (int i = 0; i < quantum; i++) {
            processoEmExecucao->status = EXECUTANDO;
            processoEmExecucao->tempoServico -= 1;
            processoEmExecucao->tempoCpuAcumulado += 1;

            // Incrementar o tempo
            tempo++;

            // Verificando se chegaram processos para criacao, ou processos retornaram de IO
            criandoProcessosParaChegada(&indice, cont, tempo, entradaProcessos, &altaPrioridade);
            retornaProcessosIOs(&IOs, &altaPrioridade, &baixaPrioridade, tempo);

            // Verificando se o processo foi finalizado
            if (processoEmExecucao->tempoServico <= 0) {
                finalizarProcesso(processoEmExecucao, tempo);
                processoEmExecucao = NULL;
                break;
            }
        
            // Verificando se o processo deve ser bloqueado 
            if ((processoEmExecucao->contIOs > processoEmExecucao->proxIO)
            && (processoEmExecucao->tempoCpuAcumulado == processoEmExecucao->temposIOs[processoEmExecucao->proxIO])) {
                bloquearProcesso(processoEmExecucao, &IOs, tempo);
                processoEmExecucao = NULL;
                break;
            }
        }
        
        // Preempcao
        if (processoEmExecucao) {
            printf(AMARELO"[T:%03d] PREEMPT    | P%d perdeu quantum (tempo restante: %d)\n"
            RESET, tempo, processoEmExecucao->PID, processoEmExecucao->tempoServico);
            preemptarProcesso(processoEmExecucao, &baixaPrioridade);
            processoEmExecucao = NULL;
        }
    }

    printf("===============================================================\n");
    printf("                      SIMULACAO FINALIZADA                     \n");
    printf("===============================================================\n");

    fclose(entrada);
    return 0;
}