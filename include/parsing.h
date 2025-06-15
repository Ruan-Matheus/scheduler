#ifndef PARSING_H
#define PARSING_H
#include "types.h"

ProcessDescriptor parseProcessos(char* buffer);
PCB criandoProcesso(ProcessDescriptor process);

#endif