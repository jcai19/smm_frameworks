PROGRAM = crc 
INCLUDEDIRS =
LIBDIRS =
LIBS =
CC = clang
CFLAGS = $(INCLUDEDIRS) -O3
LDFLAGS = $(LIBDIRS) $(LIBS) -static

FILE = crc_32.c

all: $(PROGRAM)

crc: ${FILE} Makefile
	$(CC) $(CFLAGS) ${FILE} -o crc $(LDFLAGS)

clean:
	rm -rf crc output*
