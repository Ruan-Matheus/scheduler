#ifndef TYPES_H
#define TYPES_H

#define bool int
#define true 1
#define false 0

#define BUFFER_SIZE 256
#define MAX_PROCESSOS 5
#define TIPOCHAVE PCB


typedef enum {
    DISCO,
    FITA_MAGNETICA,
    IMPRESSORA
} IO;


typedef enum {
    EXECUTANDO,
    PRONTO,
    BLOQUEADO
} status;

typedef enum {
    ALTA,
    BAIXA
} prioridade;


typedef struct {
    int numeroDoProcesso;
    int tempoDeServico;
    int tempoDeChegada;
    int contIOs;
    int tempoDeIO[MAX_PROCESSOS];
    IO tiposDeIO[MAX_PROCESSOS];
} processo;


typedef struct {
    int PID;
    int PPID;
    int prioridade;
    int tempoServico;
    int tempoChegada;
    int contIOs;
    int temposIOs[MAX_PROCESSOS];
    IO tiposIOs[MAX_PROCESSOS];
    status status;
} PCB;

#endif