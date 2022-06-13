CC=gcc
CFLAGS=-g -std=gnu11
LDFLAGS=-pthread
OBJS=main.o threadpool.o uthread.o uthread_mutex_cond.o uthread_sem.o

all: main

main: $(OBJS)

clean:
	-rm -rf main $(OBJS)
tidy: clean
	-rm -rf *~

.PHONY: all clean tidy
