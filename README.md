# mirage-solo5 -- Solo5 core platform libraries for MirageOS

**Do not install this package manually.** If you came here from a search engine looking for information on MirageOS/Solo5, [start here](https://mirage.io/wiki/install).

This package provides the [MirageOS](https://mirage.io/) `OS` library for [Solo5](https://github.com/Solo5/solo5) targets, which handles the main loop and timers. It also provides the low level C startup code and C [stubs](lib/bindings/) required by MirageOS on Solo5 targets.

The OCaml runtime and C runtime required to support it are provided separately
by the [ocaml-solo5](https://github.com/mirage/ocaml-solo5) package.

## Development

```
$ git clone https://github.com/mirage/mirage-solo5
$ cd mirage-solo5
$ opam monorepo lock
$ opam monorepo pull
$ opam install ocaml-solo5 dune
$ dune build
```
