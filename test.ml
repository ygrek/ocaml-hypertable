
open Hypertable

let printfn fmt = Printf.ksprintf print_endline fmt

let start ns =
  let cl = Client.create ~cfg:"/opt/hypertable/current/conf/hypertable.cfg" () in
  Client.open_ns cl ns

let () =
  Hypertable.init ();
  match List.tl (Array.to_list Sys.argv) with
  | "get"::ns::table::limit::key::cfs ->
    let ns = start ns in
    let limit =
      match try Some (int_of_string limit) with _ -> None with
      | Some 0 -> max_int
      | Some n -> n
      | None -> prerr_endline "E: expected integer limit"; exit 1
    in
    let table = Namespace.open_table ns table in
    let ss = ScanSpec.create () in
    ScanSpec.add_rows ss ~start:key ~ends:(key^"\xff") ~incl:true;
    List.iter (ScanSpec.add_cf ss) cfs;
    let scan = Table.scanner table ss in
    for i = 1 to limit do
      match Scanner.next scan with
      | None -> exit 0
      | Some c -> printfn "%S %s%s (%Lu) %S" c.c_row c.c_cf (match c.c_cq with None -> "" | Some s -> ":"^s) c.c_t c.c_v
    done
  | "schema"::ns::table::[] ->
    let ns = start ns in
    print_endline (Namespace.get_schema_str ns table false)
  | "list"::ns::[] ->
    let ns = start ns in
    let (ln,lt) = Namespace.get_listing ns true in
    printfn "Namespaces:";
    List.iter (fun (name,id) -> printfn "  [%s] %s" id name) ln;
    printfn "Tables:";
    List.iter (fun (name,id) -> printfn "  [%s] %s" id name) lt
  | _ ->
    prerr_endline (String.concat "\n" [
    "Available commands:";
    "  get <ns> <table> <limit> <key_prefix> [ <cf1> .. <cfN> ]";
    "  schema <ns> <table>";
    "  list <ns>";
    ])

