.PHONY: all clean
all: crc

crc: CRC.c CRC_Cfg.h CRC.h main.c
	gcc main.c CRC.c -o crc

clean:
	rm -f crc
