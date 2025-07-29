CC = gcc
CFLAGS = -O2 -Wall -I/opt/openssl-static/include
LDFLAGS = -static -L/opt/openssl-static/lib
LIBS = -lssl -lcrypto -ldl -lpthread -lz -lzstd

TARGET = http_client
SRC = http_client.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
