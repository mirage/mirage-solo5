OPAM_DIR=$(shell opam config var prefix)

CFLAGS=$(shell PKG_CONFIG_PATH=$(OPAM_DIR)/lib/pkgconfig pkg-config solo5-kernel-ukvm --cflags)
CFLAGS+=-Istubincludes -I$(OPAM_DIR)/include/mirage-xen-ocaml/include -I$(OPAM_DIR)/include/mirage-xen-posix/include

# We generate empty include files for "system" include files, by
# calculating the includes that are needed by the bindings.  The
# includes are calculated with the following command, where most of
# the -I directives are from the pkgconfig ocaml build

# /home/djwillia/opt/cross/bin/x86_64-elf-gcc -ffreestanding -E -Wp,-M main.c -I/home/djwillia/.opam/4.01.0/include/mirage-xen-ocaml/include -I/home/djwillia/.opam/4.01.0/include/mirage-xen-posix/include -I/home/djwillia/.opam/4.01.0/include -Istubincludes

OBJS=\
gnttab_stubs.o \
eventchn_stubs.o \
start_info_stubs.o \
barrier_stubs.o \
cstruct_stubs.o \
clock_stubs.o \
mm_stubs.o \
solo5_net_stubs.o \
solo5_block_stubs.o \
solo5_console_stubs.o \
solo5_undefined.o \
main.o

# exit_stubs.o \
# sched_stubs.o \
# xb_stubs.o \
# atomic_stubs.o \

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
