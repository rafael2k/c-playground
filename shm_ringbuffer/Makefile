CC=gcc
CFLAGS=-Wall -pedantic -std=c17
LDFLAGS=

.PHONY: all

all: ring_buffer1.o shm.o ring_buffer.o test-ring_buffer.o test-ring_buffer

ring_buffer1.o: ring_buffer1.c ring_buffer1.h
	$(CC) $(CFLAGS) -c ring_buffer1.c -o ring_buffer1.o

ring_buffer2.o: ring_buffer2.c ring_buffer2.h
	$(CC) $(CFLAGS) -Wno-pointer-arith -c ring_buffer2.c -o ring_buffer2.o

shm.o: shm.c shm.h
	$(CC) $(CFLAGS) -c shm.c -o shm.o


test-ring_buffer2.o: test-ring_buffer2.c
	$(CC) $(CFLAGS) -c test-ring_buffer2.c -o test-ring_buffer2.o

test-ring_buffer2: test-ring_buffer.o ring_buffer2.o
	$(CC) $(LDFLAGS) -o test-ring_buffer2 test-ring_buffer2.o ring_buffer2.o

.PHONY: clean
clean:
	rm -f *.o test-ring_buffer2
