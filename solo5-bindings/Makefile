SOLO5_DIR=/home/solo5/solo5/kernel
OPAM_DIR=/home/solo5/.opam/4.02.3

include $(SOLO5_DIR)/Makefile.solo5

S5_CFLAGS+=-Istubincludes -I$(OPAM_DIR)/include/mirage-xen-ocaml/include -I$(OPAM_DIR)/include/mirage-xen-posix/include -I$(OPAM_DIR)/include -I$(SOLO5_DIR) \
-Wno-error

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
main.o

# exit_stubs.o \
# sched_stubs.o \
# xb_stubs.o \
# atomic_stubs.o \

.PHONY: all build clean install

all: libsolo5camlbindings.a
build: libsolo5camlbindings.a

%.o: %.c $(SOLO5_DIR)/solo5.h
	$(S5_GCC) $(S5_CFLAGS) -c $< -o $@

libsolo5camlbindings.a: $(OBJS)
	$(S5_AR) rcs $@ $(OBJS)

clean:
	rm -f *.o *.a 

install:
	./install.sh
