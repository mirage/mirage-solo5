(*
 * Copyright (c) 2020 Martin Lucina <martin@lucina.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *)

external get_heap_words : unit -> int = "mirage_memory_get_heap_words"
[@@noalloc]

external get_live_words : unit -> int = "mirage_memory_get_live_words"
[@@noalloc]

external get_fast_live_words : unit -> int = "mirage_memory_get_fast_live_words"
[@@noalloc]

external get_stack_words : unit -> int = "mirage_memory_get_stack_words"
[@@noalloc]

external trim : unit -> unit = "mirage_trim_allocation" [@@noalloc]

type stat = {
  heap_words : int;
  live_words : int;
  stack_words : int;
  free_words : int;
}

let stat () =
  let h = get_heap_words () in
  let l = get_live_words () in
  let s = get_stack_words () in
  { heap_words = h; live_words = l; stack_words = s; free_words = h - l - s }

let quick_stat () =
  let h = get_heap_words () in
  let l = get_fast_live_words () in
  let s = get_stack_words () in
  { heap_words = h; live_words = l; stack_words = s; free_words = h - l - s }

let metrics ?(quick = true) ~tags () =
  let open Metrics in
  let doc = "Memory counters" in
  let stat = if quick then quick_stat else stat in
  let data () =
    let stat = stat () in
    Data.v
      [
        uint "memory heap words" stat.heap_words;
        uint "memory live words" stat.live_words;
        uint "memory stack words" stat.stack_words;
        uint "memory free words" stat.free_words;
      ]
  in
  Src.v ~doc ~tags ~data "memory"

let () = Metrics_lwt.periodically (metrics ~tags:Metrics.Tags.[] ())
