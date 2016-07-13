PREFIX ?= /usr/local

.PHONY: all clean solo5-build solo5-install solo5-uninstall
.DEFAULT: all

all:	solo5-build

solo5-build:
	cd solo5 && $(MAKE)
	cd solo5-bindings && $(MAKE) build

solo5-install:
	cd solo5 && $(MAKE) install
	cd solo5-bindings && $(MAKE) install

solo5-uninstall:
	ocamlfind remove mirage-solo5 || true
	cd solo5-bindings && $(MAKE) uninstall

clean:
	cd solo5 && $(MAKE) clean
	cd solo5-bindings && $(MAKE) clean
