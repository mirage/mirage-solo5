.PHONY: all clean
all: libmirage-solo5_bindings.a

CC=ocamlfind -toolchain freestanding ocamlopt
OBJS= clock_stubs.o mm_stubs.o main.o

libmirage-solo5_bindings.a: $(OBJS)
	$(AR) r $@ $^

clean:
	$(RM) $(OBJS) libmirage-solo5_bindings.a
