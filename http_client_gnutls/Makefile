CC = gcc
CFLAGS = -O2 -Wall
LDFLAGS = -static
LIBS = -lgnutls -lnettle -lhogweed -lgmp -lz -ldl -pthread -ltasn1 -lidn2 -lp11 -lunistring -lpsllibunistring-dev in sid

TARGET = http_client
SRC = http_client.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
