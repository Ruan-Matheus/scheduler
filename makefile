CC = gcc
CFLAGS = -std=c11 -Wall -Iinclude

all: scheduler

scheduler: src/main.c src/fila.c src/parsing.c
	$(CC) $(CFLAGS) src/main.c src/fila.c src/parsing.c -o scheduler