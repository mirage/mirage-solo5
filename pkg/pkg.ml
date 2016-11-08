#!/usr/bin/env ocaml
#use "topfind"
#require "topkg"
#require "ocb-stubblr.topkg"
open Topkg
open Ocb_stubblr_topkg

let opams = [
  Pkg.opam_file "opam" ~lint_deps_excluding:
    (Some ["ocaml-freestanding"])
]

let () =
  Pkg.describe ~build:(Pkg.build ~cmd()) ~opams "mirage-solo5" @@ fun c ->
    Ok [ Pkg.mllib "lib/mirage-solo5.mllib" ;
         Pkg.clib "lib/libmirage-solo5_bindings.clib";
         Pkg.toplevel ~dst:"pkgconfig/" "lib/native/mirage-solo5.pc" ]
