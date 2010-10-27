
open Hypertable

let () =
  Hypertable.init ();
  match List.tl (Array.to_list Sys.argv) with
  | ns::table::cfs ->
    let cl = Client.create ~cfg:"/opt/hypertable/current/conf/hypertable.cfg" () in
    let ns = Client.open_ns cl ns in
    let table = Namespace.open_table ns table in
    let ss = ScanSpec.create () in
    List.iter (ScanSpec.add_cf ss) cfs;
    let scan = Table.scanner table ss in
    while true do
      match Scanner.next scan with
      | None -> exit 0
      | Some c -> Printf.printf "%S %s%s (%Lu) %S\n" c.c_row c.c_cf (match c.c_cq with None -> "" | Some s -> ":"^s) c.c_t c.c_v
    done
  | _ ->
    Printf.eprintf "Usage: %s <ns> <table> <cf1> [<cf2> .. <cfN>]\n" Sys.executable_name

