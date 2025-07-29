CC = gcc
CFLAGS = -O2 -Wall -I/usr/include/mbedtls
LDFLAGS = -static
LIBS = -lmbedtls -lmbedx509 -lmbedcrypto

TARGET = http_client
SRC = http_client.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
