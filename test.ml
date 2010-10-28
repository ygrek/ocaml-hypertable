
open Hypertable

let () =
  Hypertable.init ();
  match List.tl (Array.to_list Sys.argv) with
  | ns::table::key::cfs ->
    let cl = Client.create ~cfg:"/opt/hypertable/current/conf/hypertable.cfg" () in
    let ns = Client.open_ns cl ns in
    let table = Namespace.open_table ns table in
    let ss = ScanSpec.create () in
    ScanSpec.add_rows ss ~start:key ~ends:(key^"\xff") ~incl:true;
    List.iter (ScanSpec.add_cf ss) cfs;
    let scan = Table.scanner table ss in
    while true do
      match Scanner.next scan with
      | None -> exit 0
      | Some c -> Printf.printf "%S %s%s (%Lu) %S\n" c.c_row c.c_cf (match c.c_cq with None -> "" | Some s -> ":"^s) c.c_t c.c_v
    done
  | _ ->
    Printf.eprintf "Usage: %s <ns> <table> <key_prefix> [ <cf1> .. <cfN> ]\n" Sys.executable_name

