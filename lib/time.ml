(* Lightweight thread library for Objective Caml
 * http://www.ocsigen.org/lwt
 * Module Lwt_mirage, based on Lwt_unix
 * Copyright (C) 2010 Anil Madhavapeddy
 * Copyright (C) 2005-2008 Jérôme Vouillon
 * Laboratoire PPS - CNRS Université Paris Diderot
 *                    2009 Jérémie Dimino
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

module SleepQueue = Binary_heap.Make (struct
  type t = Mirage_sleep.t

  let compare Mirage_sleep.{ time = t1; _ } Mirage_sleep.{ time = t2; _ } =
    compare t1 t2
end)

(* Threads waiting for a timeout to expire: *)
let sleep_queue =
  let dummy =
    Mirage_sleep.
      { time = time (); canceled = false; thread = Lwt.wait () |> snd }
  in
  SleepQueue.create ~dummy 0

let sleep_metrics =
  let open Metrics in
  let doc = "Sleep queue size" in
  let data () =
    let q_size = SleepQueue.length sleep_queue in
    Data.v [ uint "sleep queue size" q_size ]
  in
  Src.v ~doc ~tags:Metrics.Tags.[] ~data "sleep"

let m () = Metrics.add sleep_metrics (fun x -> x) (fun d -> d ())
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
  List.iter
    (fun e -> SleepQueue.add sleep_queue e)
    (Mirage_sleep.new_sleepers ());
  m ();
  get_next_timeout ()
