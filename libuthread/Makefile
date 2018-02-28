# Target library
lib := libuthread.a
CC=gcc
CFLAGS= -Wall -Werror -g

all: $(lib)

preempt.o: preempt.c
	$(CC) $(CFLAGS) -c preempt.c preempt.h

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c queue.h

context.o: context.c
	$(CC) $(CFLAGS) -c context.c context.h

uthread.o: uthread.c
	$(CC) $(CFLAGS) -c uthread.c uthread.h

libuthread.a: queue.o context.o uthread.o preempt.o
	ar rcs libuthread.a queue.o context.o uthread.o preempt.o

clean:
	rm -f *.o *.gch
