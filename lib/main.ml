(* Lightweight thread library for Objective Caml
 * http://www.ocsigen.org/lwt
 * Module Lwt_main
 * Copyright (C) 2009 Jérémie Dimino
 * Copyright (C) 2010 Anil Madhavapeddy <anil@recoil.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, with linking exceptions;
 * either version 2.1 of the License, or (at your option) any later
 * version. See COPYING file for details.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *)

open Lwt

external solo5_yield : [`Time] Time.Monotonic.t -> bool = "mirage_solo5_yield"

let exit_hooks = Lwt_sequence.create ()
let enter_hooks = Lwt_sequence.create ()
let exit_iter_hooks = Lwt_sequence.create ()
let enter_iter_hooks = Lwt_sequence.create ()

let rec call_hooks hooks  =
  match Lwt_sequence.take_opt_l hooks with
    | None ->
        return ()
    | Some f ->
        (* Run the hooks in parallel *)
        let _ =
          Lwt.catch f
          (fun exn ->
            Printf.printf "ERROR: call_hooks(): Unhandled exception: %s\n%!" (Printexc.to_string exn);
            return ()) in
        call_hooks hooks

(* Solo5 currently has an all-or-nothing interface to block and wait for I/O
 * events, so we use a single condition variable to block threads which are
 * waiting for work and wake them all up if I/O is possible.  This will need to
 * be extended once solo5_yield() gains support for selecting events of
 * interest. *)
let work = Lwt_condition.create ()
let wait_for_work () = Lwt_condition.wait work

let err exn =
  Logs.err (fun m -> m "main: %s\n%s" (Printexc.to_string exn) (Printexc.get_backtrace ())) ;
  exit 1

(* Execute one iteration and register a callback function *)
let run t =
  let t = call_hooks enter_hooks <&> t in
  let rec aux () =
    Lwt.wakeup_paused ();
    Time.restart_threads Time.Monotonic.time;
    match (try Lwt.poll t with exn -> err exn) with
    | Some () ->
        ()
    | None ->
        let timeout =
          match Time.select_next () with
          |None -> Time.Monotonic.(time () + of_nanoseconds 86_400_000_000_000L) (* one day = 24 * 60 * 60 s *)
          |Some tm -> tm
        in
        if solo5_yield timeout then begin
          (* Call enter hooks. *)
          Lwt_sequence.iter_l (fun f -> f ()) enter_iter_hooks;
          (* Some I/O is possible, wake up threads and continue. *)
          Lwt_condition.broadcast work ();
          (* Call leave hooks. *)
          Lwt_sequence.iter_l (fun f -> f ()) exit_iter_hooks;
          aux ()
        end else begin
          aux ()
        end in
  aux ()

let () = at_exit (fun () -> run (call_hooks exit_hooks))
let at_exit f = ignore (Lwt_sequence.add_l f exit_hooks)
let at_enter f = ignore (Lwt_sequence.add_l f enter_hooks)
let at_exit_iter f = ignore (Lwt_sequence.add_l f exit_iter_hooks)
let at_enter_iter f = ignore (Lwt_sequence.add_l f enter_iter_hooks)
