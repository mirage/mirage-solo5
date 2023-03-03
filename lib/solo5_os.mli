module Lifecycle : sig
  val await_shutdown_request :
    ?can_poweroff:bool ->
    ?can_reboot:bool ->
    unit ->
    [ `Poweroff | `Reboot ] Lwt.t
  (** [await_shutdown_request ()] is thread that resolves when the domain is
      asked to shut down. The optional [poweroff] (default:[true]) and [reboot]
      (default:[false]) arguments can be used to indicate which features the
      caller wants to advertise (however, you can still get a request for a mode
      you didn't claim to support). *)
end

module Main : sig
  val wait_for_work_on_handle : int64 -> unit Lwt.t
  val run : unit Lwt.t -> unit
end

module Memory : sig
  (** Memory management operations. *)

  type stat = {
    heap_words : int;  (** total number of words allocatable on the heap. *)
    live_words : int;  (** number of live (i.e. allocated) words on the heap. *)
    stack_words : int;
        (** number of words in use by the program stack. This includes any space
            reserved by a stack guard. *)
    free_words : int;
        (** number of free (i.e. allocatable) words on the heap. *)
  }
  (** Memory allocation statistics. Units are the system word size, as used by
      the OCaml stdlib Gc module. *)

  val stat : unit -> stat
  (** [stat ()] returns memory allocation statistics. This uses mallinfo and
      walks over the entire heap. This call is slower than {!quick_stat}. *)

  val quick_stat : unit -> stat

  (** [quick_stat ()] returns memory allocation statistics. This call uses a
      precomputed value. This call is cheaper than {!stat}, but the returned
      values may not be completely accurate. *)

  val trim : unit -> unit
  (** [trim ()] release free memory from the heap (may update the value returned
      by {!quick_stat}) *)

  val metrics :
    ?quick:bool ->
    tags:'a Metrics.Tags.t ->
    unit ->
    ('a, unit -> Metrics.Data.t) Metrics.src
  (** [metrics ~quick ~tags] is a metrics source calling {quick_stat} (unless
      [quick] is set to [false]) or {stat}. By default, this metrics source is
      registered with [Metrics_lwt.periodically] (with [quick] set to [true]. *)
end

module Time : sig
  val sleep_ns : int64 -> unit Lwt.t
  (** [sleep_ns d] Block the current thread for [n] nanoseconds. *)
end

module Solo5 : sig
  type solo5_result =
    | SOLO5_R_OK
    | SOLO5_R_AGAIN
    | SOLO5_R_EINVAL
    | SOLO5_R_EUNSPEC  (** A type mapping the C enum solo5_result_t to OCaml **)
end
