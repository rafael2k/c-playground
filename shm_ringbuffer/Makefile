CC=gcc
CFLAGS=-Wall -pedantic -std=c17
# CFLAGS99=-Wall -pedantic -std=c99
LDFLAGS=

.PHONY: all

all: buffer2.o ring_buffer1.o shm_sysv.o shm_posix.o ring_buffer2.o test-ring_buffer2.o test-ring_buffer2 test-ring_buffer1.o test-ring_buffer1

ring_buffer1.o: ring_buffer1.c ring_buffer1.h
	$(CC) $(CFLAGS) -c ring_buffer1.c -o ring_buffer1.o

ring_buffer2.o: ring_buffer2.c ring_buffer2.h
	$(CC) $(CFLAGS) -Wno-pointer-arith -c ring_buffer2.c -o ring_buffer2.o

# API
buffer2.o: buffer2.c buffer2.h
	$(CC) $(CFLAGS) -c buffer2.c -o buffer2.o

# shared memory helpers
shm_sysv.o: shm_sysv.c shm_sysv.h
	$(CC) $(CFLAGS) -c shm_sysv.c -o shm_sysv.o

shm_posix.o: shm_posix.c shm_posix.h
	$(CC) $(CFLAGS) -c shm_posix.c -o shm_posix.o

# tests...
test-ring_buffer2.o: test-ring_buffer2.c
	$(CC) $(CFLAGS) -c test-ring_buffer2.c -o test-ring_buffer2.o

test-ring_buffer2: test-ring_buffer2.o ring_buffer2.o
	$(CC) $(LDFLAGS) -o test-ring_buffer2 test-ring_buffer2.o ring_buffer2.o

test-ring_buffer1.o: test-ring_buffer1.c
	$(CC) $(CFLAGS) -c test-ring_buffer1.c -o test-ring_buffer1.o

test-ring_buffer1: test-ring_buffer1.o ring_buffer1.o shm_sysv.o
	$(CC) $(LDFLAGS) -o test-ring_buffer1 test-ring_buffer1.o ring_buffer1.o shm_sysv.o



.PHONY: clean
clean:
	rm -f *.o test-ring_buffer2 test-ring_buffer1
