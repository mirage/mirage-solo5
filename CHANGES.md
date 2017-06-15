## v0.2.1 (2017-06-15)

* Use [@@noalloc] rather than "noalloc"
* Fix warnings, formatting and cleanup
* Require OCaml 4.03
* Test also with OCaml 4.04
* Rename caml_{alloc_pages,get_addr} to mirage_{alloc_pages,get_addr}
  to avoid using the OCaml compiler namespace. This corresponds with
  [mirage/io-page#48]

## v0.2.0 (2017-01-17)

* Port to topkg (@mato, @hannesm, @yomimono, #15)
* Minor fixes and cleanups (various)

## v0.1.1 (2016-07-21)

* Initial release for publishing to opam.ocaml.org.
