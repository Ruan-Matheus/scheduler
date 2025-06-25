#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsing.h"

int pid_counter = 1;
int ppid_counter = 0;

ProcessDescriptor parseProcessos(char* buffer) {
    ProcessDescriptor novo;
    novo.contIOs = 0; // Sempre inicialize para garantir que não haja lixo

    char *fields[5] = {NULL}; // Array para guardar os 5 campos da linha CSV
    int field_count = 0;
    char *token;

    // --- FASE 1: Dividir a linha por vírgulas e salvar os ponteiros ---
    buffer[strcspn(buffer, "\r\n")] = '\0'; // Remove quebra de linha

    token = strtok(buffer, ",");
    while (token != NULL && field_count < 5) {
        fields[field_count++] = token;
        token = strtok(NULL, ",");
    }

    // --- FASE 2: Processar cada campo salvo ---

    // Campo 0: Número do processo
    if (fields[0]) novo.numeroDoProcesso = atoi(fields[0]);

    // Campo 1: Tempo de serviço
    if (fields[1]) novo.tempoDeServico = atoi(fields[1]);
    
    // Campo 2: Tempo de chegada
    if (fields[2]) novo.tempoDeChegada = atoi(fields[2]);

    // Campo 3: Tempos de I/O (agora é seguro usar strtok novamente)
    if (fields[3]) {
        int i = 0;
        char *split = strtok(fields[3], " ");
        while (split != NULL) {
            novo.tempoDeIO[i++] = atoi(split);
            split = strtok(NULL, " ");
        }
        novo.contIOs = i;
    }

    // Campo 4: Tipos de I/O (também seguro usar strtok)
    if (fields[4] && novo.contIOs > 0) {
        int j = 0;
        char* split = strtok(fields[4], " ");
        while (split != NULL) {
            if (strcmp(split, "A") == 0)      novo.tiposDeIO[j++] = DISCO;
            else if (strcmp(split, "B") == 0) novo.tiposDeIO[j++] = FITA_MAGNETICA;
            else if (strcmp(split, "C") == 0) novo.tiposDeIO[j++] = IMPRESSORA;
            split = strtok(NULL, " ");
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