CC=gcc

CPPFLAGS = -D_DEFAULT_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS   = -O3 -g -std=c99 -Wall -I. -Iinclude -Ideps/oblas
CFLAGS  += -funroll-loops -ftree-vectorize -fno-inline -fstack-protector-all


ifeq (${uname_p},aarch64)
	OBLAS_CPPFLAGS="-DOBLAS_NEON"
# aarch64 Raspberry Pi 4 or better
	CFLAGS+=-moutline-atomics -march=armv8-a+crc -I/usr/include/iniparser
# for Pi 5 use:
#	CFLAGS+=-march=armv8.2-a+crypto+fp16+rcpc+dotprod
else
	OBLAS_CPPFLAGS="-DOBLAS_AVX -DOCTMAT_ALIGN=32"
# x86_64 with SSE 4.2 level or better
	CFLAGS+=-march=x86-64-v2
endif


OBJ=\
lib/bitmask.o\
lib/io.o\
lib/params.o\
lib/precode.o\
lib/rand.o\
lib/sched.o\
lib/spmat.o\
lib/tuple.o\
lib/wrkmat.o\
lib/nanorq.o

all: test libnanorq.a

test: encode decode
	$(MAKE) -f example.make

encode: encode.o libnanorq.a 

decode: decode.o libnanorq.a

benchmark: benchmark.c libnanorq.a
	$(CC) $(CFLAGS) benchmark.c -o $@ libnanorq.a $(LDLIBS)

deps/oblas/liboblas.a:
	$(MAKE) -C deps/oblas CPPFLAGS+=$(OBLAS_CPPFLAGS)

.PHONY: oblas_clean
oblas_clean:
	$(MAKE) -C deps/oblas clean

libnanorq.a: $(OBJ) deps/oblas/liboblas.a
	$(AR) rcs $@ $(OBJ) deps/oblas/*.o

clean: oblas_clean
	$(RM) encode decode lib/*.o *.o *.a *.gcda *.gcno *.gcov callgrind.* *.gperf *.prof *.heap perf.data perf.data.old
