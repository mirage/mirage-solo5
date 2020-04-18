module C = Configurator.V1

let ( / ) = Filename.concat

let package = "ocaml-freestanding"

let () =
  C.main ~name:"foo" (fun c ->
      let opam_prefix =
        String.trim
          (C.Process.run_capture_exn c "opam" [ "config"; "var"; "prefix" ])
      in
      let dune_prefix =
        let root =
          try Sys.getenv "INSIDE_DUNE" with Not_found -> assert false
        in
        let context = Filename.basename root in
        let root = Filename.dirname root in
        root / "install" / context
      in
      let path prefix = prefix / "lib" / "pkgconfig" in
      let pkg_config_path = [ path opam_prefix; path dune_prefix ] in
      Unix.putenv "PKG_CONFIG_PATH" (String.concat ":" pkg_config_path);
      let conf =
        match C.Pkg_config.get c with
        | None -> failwith "cannot find pkg-config"
        | Some pc -> (
            match C.Pkg_config.query pc ~package with
            | None -> failwith "cannot find solo5's pkg-config info"
            | Some deps -> deps )
      in
      C.Flags.write_sexp "cflags" conf.cflags)
