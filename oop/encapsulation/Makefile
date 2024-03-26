##
# OOP in C, encapsulation
#
# @file
# @version 0.1

all: shape

shape: shape.o main.o
	$(CC) -o shape shape.o main.o -lc

shape.o: shape.c
	$(CC) -c shape.c -o shape.o

main.o: main.c
	$(CC) -c main.c -o main.o

.PHONY: clean
clean:
	rm -f main.o shape.o shape



# end
