let app () =
  let open Lwt.Syntax in
  Printf.printf "Hello!\n%!";
  let* () = OS.Time.sleep_ns 1_000_000_000L in
  Printf.printf "After 1s\n%!";
  let+ () = OS.Time.sleep_ns 1_000_000_000L in
  Printf.printf "After 2s\n%!"

let () = OS.Main.run (app ())
