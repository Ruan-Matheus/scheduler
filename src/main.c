#include <stdio.h>
#include <stdlib.h>
#include "fila.h"
#include "types.h"
#include "parsing.h"

#define QUANTUM_TEMPO 2
const char* stringsIO[] = { "DISCO", "FITA", "IMPRESSORA" };


int main(int c, char** argv) {
    FILA baixaPrioridade;
    FILA altaPrioridade;
    FILA IOs;
    int quantum = QUANTUM_TEMPO;
    int tempo = 0;
    processo entradaProcessos[MAX_PROCESSOS];
    char bufferProcessos[BUFFER_SIZE];
    char* arquivoEntrada = "processos_entrada.csv";
    bool maisEntrada = true;
    
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
    while (maisEntrada ||
           !vazioFila(baixaPrioridade) ||
           !vazioFila(altaPrioridade) ||
           !vazioFila(IOs)) {

        
        if (maisEntrada) {
            // Checar o tempo de chegada dos processos do topo, para criar o processo no tempo certo
            if (indice < cont) {
                PCB novoProcesso = criandoProcesso(entradaProcessos[indice]);
                
                printf("PID: %d ", novoProcesso.PID);
                for (int i = 0; i < novoProcesso.contIOs; i++) {
                    printf("%s ", stringsIO[novoProcesso.tiposIOs[i]]);
                }
                printf("\n");
                
                //enqueue(&altaPrioridade, novoProcesso);
                indice++;
            }

            else {
                // fgets retornou NULL ou EOF
                maisEntrada = false;
            }
        }

        // 2) Verificar retornos de I/O

        // 3) Executar / pré-emptar / bloquear o processo atual

        // Incrementar o tempo
        tempo += quantum;
    }

    fclose(entrada);
    return 0;
}


