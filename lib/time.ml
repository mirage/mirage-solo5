(* Lightweight thread library for Objective Caml
 * http://www.ocsigen.org/lwt
 * Module Lwt_mirage, based on Lwt_unix
 * Copyright (C) 2010 Anil Madhavapeddy
 * Copyright (C) 2005-2008 J�r�me Vouillon
 * Laboratoire PPS - CNRS Universit� Paris Diderot
 *                    2009 J�r�mie Dimino
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

external time : unit -> int64 = "caml_get_monotonic_time"

type t = int64

(* +-----------------------------------------------------------------+
   | Sleepers                                                        |
   +-----------------------------------------------------------------+ *)

type sleep = { time : t; mutable canceled : bool; thread : unit Lwt.u }

module SleepQueue = Binary_heap.Make (struct
  type t = sleep

  let compare { time = t1; _ } { time = t2; _ } = compare t1 t2
end)

(* Threads waiting for a timeout to expire: *)
let sleep_queue =
  let dummy =
    { time = time (); canceled = false; thread = Lwt.wait () |> snd }
  in
  SleepQueue.create ~dummy 0

(* Sleepers added since the last iteration of the main loop:

   They are not added immediatly to the main sleep queue in order to
   prevent them from being wakeup immediatly by [restart_threads].
*)
let new_sleeps = ref []

let sleep_metrics =
  let open Metrics in
  let doc = "Sleep queue size" in
  let data () =
    let q_size = SleepQueue.length sleep_queue
    and new_size = List.length !new_sleeps in
    Data.v
      [
        uint "sleep queue size" q_size;
        uint "new sleeper size" new_size;
        uint "total sleeper size" (q_size + new_size);
      ]
  in
  Src.v ~doc ~tags:Metrics.Tags.[] ~data "sleep"

let m () = Metrics.add sleep_metrics (fun x -> x) (fun d -> d ())

let sleep_ns d =
  let res, w = Lwt.task () in
  let t = Int64.add (time ()) d in
  let sleeper = { time = t; canceled = false; thread = w } in
  new_sleeps := sleeper :: !new_sleeps;
  m ();
  Lwt.on_cancel res (fun _ -> sleeper.canceled <- true);
  res

let in_the_past now t = t = 0L || t <= now ()

let rec restart_threads now =
  match SleepQueue.minimum sleep_queue with
  | exception Binary_heap.Empty -> ()
  | { canceled = true; _ } ->
      SleepQueue.remove sleep_queue;
      m ();
      restart_threads now
  | { time; thread; _ } when in_the_past now time ->
      SleepQueue.remove sleep_queue;
      m ();
      Lwt.wakeup thread ();
      restart_threads now
  | _ -> ()

(* +-----------------------------------------------------------------+
   | Event loop                                                      |
   +-----------------------------------------------------------------+ *)

let rec get_next_timeout () =
  match SleepQueue.minimum sleep_queue with
  | exception Binary_heap.Empty -> None
  | { canceled = true; _ } ->
      SleepQueue.remove sleep_queue;
      m ();
      get_next_timeout ()
  | { time; _ } -> Some time

let select_next () =
  (* Transfer all sleepers added since the last iteration to the main
     sleep queue: *)
  List.iter (fun e -> SleepQueue.add sleep_queue e) !new_sleeps;
  new_sleeps := [];
  m ();
  get_next_timeout ()
