#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"

int pid_counter = 1;
int ppid_counter = 0;

ProcessDescriptor parseProcessos(char* buffer) {
    ProcessDescriptor novo;
    char *token;
    char *split;

    // Número do processo
    token = strtok(buffer, ",");
    novo.numeroDoProcesso = atoi(token);

    // Tempo de serviço
    token = strtok(NULL, ",");
    novo.tempoDeServico = atoi(token);

    // Tempo de chegada
    token = strtok(NULL, ",");
    novo.tempoDeChegada = atoi(token);

    // Tempos de IOs
    token = strtok(NULL, ",");
    if (token && *token != '\0') {
        int i = 0;
        split = strtok(token, " ");
        while (split) {
            novo.tempoDeIO[i++] = atoi(split);
            split = strtok(NULL, " ");
        }
        novo.contIOs = i;
    } else {
        novo.contIOs = 0;
    }

    // Tipos de IOs
    token = strtok(NULL, ",");
    if (token && *token != '\0') {
        token[strcspn(token, "\r\n")] = '\0';
        int j = 0;
        split = strtok(token, " ");
        while (split) {
            if (strcmp(split, "A") == 0)      novo.tiposDeIO[j++] = DISCO;
            else if (strcmp(split, "B") == 0)   novo.tiposDeIO[j++] = FITA_MAGNETICA;
            else if (strcmp(split, "C") == 0)   novo.tiposDeIO[j++] = IMPRESSORA;
            split = strtok(NULL, " ");
        }
    }

    return novo;
}

PCB criandoProcesso(ProcessDescriptor process) {
    PCB p;
    p.PPID = ppid_counter++;
    p.PID = pid_counter++;
    p.status = PRONTO;
    p.prioridade = ALTA;
    p.tempoChegada = process.tempoDeChegada;
    p.tempoServico = process.tempoDeServico;
    p.contIOs = process.contIOs;
    p.tempoDeRetornoIO = 0;
    p.proxIO = 0;

    for (int i = 0; i < process.contIOs; i++) {
        p.tiposIOs[i] = process.tiposDeIO[i];
        p.temposIOs[i] = process.tempoDeIO[i];
    }

    return p;
}