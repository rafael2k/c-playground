CC=gcc
CFLAGS=-Wall -pedantic -std=c17
LDFLAGS=

all: buffer.o shm.o ring_buffer.o

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c -o buffer.o

shm.o: shm.c shm.h
	$(CC) $(CFLAGS) -c shm.c -o shm.o

ring_buffer.o: ring_buffer.c ring_buffer.h
	$(CC) $(CFLAGS) -Wno-pointer-arith -c ring_buffer.c -o ring_buffer.o



.PHONY:clean
clean:
	rm -f *.o
