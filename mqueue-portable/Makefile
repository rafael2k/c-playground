CC=gcc
CFLAGS=-Wall -pthread -std=c11
LDFLAGS=-lpthread -lrt

all: tester

tester: mqueue.o tester.o
	$(CC) mqueue.o tester.o -o tester $(LDFLAGS)

mqueue.o: mqueue.c mqueue.h
	$(CC) $(CFLAGS) -c mqueue.c -o mqueue.o

tester.o: tester.c
	$(CC) $(CFLAGS) -c tester.c -o tester.o

clean:
	rm -f *.o tester
