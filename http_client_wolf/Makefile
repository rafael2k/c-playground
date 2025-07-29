CC = gcc
CFLAGS = -O2 -Wall -I/usr/local/include
LDFLAGS = -L/usr/local/lib -static
LIBS = -lwolfssl -lm

TARGET = http_client
SRC = http_client.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
