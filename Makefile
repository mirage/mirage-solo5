.PHONY: build clean doc

build:
	dune build

clean:
	dune clean

doc:
	dune build @doc