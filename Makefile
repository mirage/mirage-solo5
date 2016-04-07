MIRAGE_OS ?= unix

PREFIX ?= /usr/local

.PHONY: all build clean install test
.DEFAULT: all

all:	build
	@ :

build:
	cd $(MIRAGE_OS) && $(MAKE) all MIRAGE_OS=$(MIRAGE_OS)

clean:
	cd $(MIRAGE_OS) && $(MAKE) clean MIRAGE_OS=$(MIRAGE_OS)

install:
	cd $(MIRAGE_OS) && $(MAKE) install MIRAGE_OS=$(MIRAGE_OS)

uninstall:
	cd $(MIRAGE_OS) && $(MAKE) uninstall MIRAGE_OS=$(MIRAGE_OS)

test:
	cd $(MIRAGE_OS) && $(MAKE) test MIRAGE_OS=$(MIRAGE_OS)

doc:
	cd $(MIRAGE_OS) && $(MAKE) doc MIRAGE_OS=$(MIRAGE_OS)

unix-%:
	$(MAKE) MIRAGE_OS=unix PREFIX=$(PREFIX) $*

solo5-build:
	cd solo5 && $(MAKE)
	cd solo5-bindings && $(MAKE) build

xen-build:
	cd xen && $(MAKE)
	cd bindings && $(MAKE) build

solo5-install:
	cd solo5 && $(MAKE) install
	cd solo5-bindings && $(MAKE) install

xen-install:
	cd xen && $(MAKE) install
	cd bindings && $(MAKE) install

solo5-uninstall:
	ocamlfind remove mirage-solo5 || true
	cd solo5-bindings && $(MAKE) uninstall

xen-uninstall:
	ocamlfind remove mirage-xen || true
	cd bindings && $(MAKE) uninstall

xen-ocaml-build:
	cd xen-ocaml && $(MAKE) build

xen-ocaml-install:
	cd xen-ocaml && $(MAKE) install

xen-ocaml-uninstall:
	cd xen-ocaml && $(MAKE) uninstall

xen-posix-build:
	cd xen-posix && $(MAKE) build

xen-posix-install:
	cd xen-posix && $(MAKE) install

xen-posix-uninstall:
	cd xen-posix && $(MAKE) uninstall

IMAGE = mirage-xen

test-xen-depends: Dockerfile build.sh
	docker build -t $(IMAGE) .

test-xen-build: test-xen-depends clean
	docker run -v $(shell pwd):/src $(IMAGE) /build.sh

VERSION = $(shell grep 'VERSION=' unix/_vars | sed 's/VERSION=*//')
ARCHIVE = https://github.com/mirage/mirage-platform/archive/v$(VERSION).tar.gz

release:
	git tag -a v$(VERSION) -m "Version $(VERSION)."
	git push upstream v$(VERSION)
	$(MAKE) pr

export OPAMYES=1

pr:
	opam pin add mirage-$(MIRAGE_OS) . -n
	opam publish prepare mirage-$(MIRAGE_OS).$(VERSION) $(ARCHIVE)
	opam unpin mirage-$(MIRAGE_OS) -n
	opam publish submit mirage-$(MIRAGE_OS).$(VERSION)
