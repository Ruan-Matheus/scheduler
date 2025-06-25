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
    char *saveptr1;
    char *saveptr2;

    // Inicializa o contador de I/Os para evitar lixo de memória
    novo.contIOs = 0;

    buffer[strcspn(buffer, "\r\n")] = '\0';

    // Número do processo
    token = strtok_r(buffer, ",", &saveptr1);
    if (token) novo.numeroDoProcesso = atoi(token);

    // Tempo de serviço
    token = strtok_r(NULL, ",", &saveptr1);
    if (token) novo.tempoDeServico = atoi(token);

    // Tempo de chegada
    token = strtok_r(NULL, ",", &saveptr1);
    if (token) novo.tempoDeChegada = atoi(token);

    // Tempos de IOs
    token = strtok_r(NULL, ",", &saveptr1);
    // CORREÇÃO: Removida a verificação incorreta '*token != ' ''
    if (token && token[0] != '\0' && token[0] != ' ') {
        int i = 0;
        for (split = strtok_r(token, " ", &saveptr2); split; split = strtok_r(NULL, " ", &saveptr2)) {
            novo.tempoDeIO[i++] = atoi(split);
        }
        novo.contIOs = i;
    }

    // Tipos de IOs
    token = strtok_r(NULL, ",", &saveptr1);
    // CORREÇÃO: Removida a verificação incorreta '*token != ' ''
    if (token && token[0] != '\0' && token[0] != ' ') {
        int j = 0;
        for (split = strtok_r(token, " ", &saveptr2); split; split = strtok_r(NULL, " ", &saveptr2)) {
            if (strcmp(split, "A") == 0)      novo.tiposDeIO[j++] = DISCO;
            else if (strcmp(split, "B") == 0) novo.tiposDeIO[j++] = FITA_MAGNETICA;
            else if (strcmp(split, "C") == 0) novo.tiposDeIO[j++] = IMPRESSORA;
        }
    }

    return novo;
}



PCB criandoProcesso(ProcessDescriptor process) {
    PCB p;
    memset(&p, 0, sizeof(PCB)); 
    p.PPID = ppid_counter++;
    p.PID = pid_counter++;
    p.status = PRONTO;
    p.prioridade = ALTA;
    p.tempoChegada = process.tempoDeChegada;
    p.tempoServico = process.tempoDeServico;
    p.tempoCpuAcumulado = 0;
    p.contIOs = process.contIOs;
    p.tempoDeRetornoIO = 0;
    p.proxIO = 0;

    for (int i = 0; i < process.contIOs; i++) {
        p.tiposIOs[i] = process.tiposDeIO[i];
        p.temposIOs[i] = process.tempoDeIO[i];
    }

    return p;
}