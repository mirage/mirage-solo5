OPAM_DIR=$(shell opam config var prefix)

FREESTANDING_CFLAGS=$(shell PKG_CONFIG_PATH=$(OPAM_DIR)/lib/pkgconfig pkg-config ocaml-freestanding --cflags)
CC=gcc
CFLAGS=-O2 -g -Wall -Werror $(FREESTANDING_CFLAGS)

OBJS=\
alloc_pages_stubs.o \
barrier_stubs.o \
checksum_stubs.o \
clock_stubs.o \
cstruct_stubs.o \
mm_stubs.o \
solo5_net_stubs.o \
solo5_block_stubs.o \
solo5_console_stubs.o \
main.o

.PHONY: all build clean install

all: libsolo5camlbindings.a
build: libsolo5camlbindings.a

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

libsolo5camlbindings.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -f *.o *.a 

install:
	./install.sh
