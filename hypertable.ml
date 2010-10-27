(** *)

exception Error of string

module Key = struct
type t = { row : string; cf : string; cq : string; t : int64; }
end

module Mutator = struct
type t
external set_key : t -> Key.t -> string -> unit = "caml_hypertable_tmut_set_key"
external set : t -> r:string -> cf:string -> ?cq:string option (* -> ?t:int64 option *) -> string -> unit = "caml_hypertable_tmut_set"
external flush : t -> unit = "caml_hypertable_tmut_flush"
end

type cell = { c_row : string; c_cf : string; c_cq : string option; c_t : int64; c_v : string; c_flag : int; }

module ScanSpec = struct
type t
external create : unit -> t = "caml_hypertable_scanspec_create"
external add_cf : t -> string -> unit = "caml_hypertable_scanspec_add_cf"
external add_row : t -> string -> unit = "caml_hypertable_scanspec_add_row"
external add_rows : t -> start:string -> ends:string -> incl:bool -> unit = "caml_hypertable_scanspec_add_rows"
end

module Scanner = struct
type t
external next : t -> cell option = "caml_hypertable_tscan_next"
end

module Table = struct
type t
external mutator : t -> int -> int -> int -> Mutator.t = "caml_hypertable_table_create_mutator"
let mutator ?(timeout=0) ?(flags=0) ?(flush=0) t = mutator t timeout flags flush
external scanner : t -> ScanSpec.t -> int -> bool -> Scanner.t = "caml_hypertable_table_create_scanner"
let scanner t ?(timeout=0) ?(retry=false) scanspec = scanner t scanspec timeout retry
end

module Namespace = struct
type t
external open_table : t -> string -> bool -> Table.t = "caml_hypertable_ns_open_table"
let open_table ns ?(force=false) name = open_table ns name force
end

module Client = struct
type t
external create : ?dir:string -> ?cfg:string -> ?timeout:int -> unit -> t = "caml_hypertable_client_create"
external open_ns : t -> ?base:Namespace.t -> string -> Namespace.t = "caml_hypertable_client_open_ns"
end

let init () =
  Callback.register_exception "Hypertable.Error" (Error "")

let () =
  init ()
