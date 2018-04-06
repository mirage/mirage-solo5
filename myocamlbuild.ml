open Ocamlbuild_plugin

let () =
  dispatch begin fun hook ->
    Ocb_stubblr.init hook ;
    match hook with
    | After_rules -> flag [ "ocamlmklib" ; "custom" ] & A "-custom"
    | _ -> ()
  end
