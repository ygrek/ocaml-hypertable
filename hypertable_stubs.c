
#include "cxx_wrapped.h"
#include <Common/Compat.h>
#include <Hypertable/Lib/Client.h>

using namespace Hypertable;

template<> char const* wrapped<Client>::ml_name() { return "hypertable.Client"; }
template<> char const* wrapped<Namespace>::ml_name() { return "hypertable.Namespace"; }
template<> char const* wrapped<Table>::ml_name() { return "hypertable.Table"; }
template<> char const* wrapped<TableMutator>::ml_name() { return "hypertable.TableMutator"; }
template<> char const* wrapped<TableScanner>::ml_name() { return "hypertable.TableScanner"; }
template<> char const* wrapped<ScanSpecBuilder>::ml_name() { return "hypertable.ScanSpecBuilder"; }

extern "C" {

#include <caml/callback.h>
#include <caml/signals.h>

#define Val_none Val_int(0)

static value Val_some(value v)
{
    CAMLparam1(v);
    CAMLlocal1(some);
    some = caml_alloc_small(1, 0);
    Field(some, 0) = v;
    CAMLreturn(some);
}

#define Some_val(v) Field(v,0)

#define Bytes_val(v) (&Byte_u(v,0))

/*
static value val_of_string(String const& s)
{
  CAMLparam0();
  CAMLlocal1(v);
  v = caml_alloc_string(s.size());
  memcpy(String_val(v), s.c_str(), s.size());
  CAMLreturn(v);
}
*/

static String string_of_val(value v)
{
  String s;
  s.assign(String_val(v), caml_string_length(v));
  return s;
}

#define CAML_HT_F1(name, v1) \
CAMLprim value caml_hypertable_##name(value v1) \
{ \
  CAMLparam1(v1); \
  try

#define CAML_HT_F2(name, v1, v2) \
CAMLprim value caml_hypertable_##name(value v1, value v2) \
{ \
  CAMLparam2(v1,v2); \
  try

#define CAML_HT_F3(name, v1, v2, v3) \
CAMLprim value caml_hypertable_##name(value v1, value v2, value v3) \
{ \
  CAMLparam3(v1,v2,v3); \
  try

#define CAML_HT_F4(name, v1, v2, v3, v4) \
CAMLprim value caml_hypertable_##name(value v1, value v2, value v3, value v4) \
{ \
  CAMLparam4(v1,v2,v3,v4); \
  try

#define CAML_HT_F5(name, v1, v2, v3, v4, v5) \
CAMLprim value caml_hypertable_##name(value v1, value v2, value v3, value v4, value v5) \
{ \
  CAMLparam5(v1,v2,v3,v4,v5); \
  try

#define CAML_HT_END \
  catch(Exception& e) \
  { \
    raise_error(e); \
    assert(false); \
    CAMLreturn(Val_unit); \
  } \
}

static void raise_error(Exception& e) 
{
  static value* exn = NULL;
  if (NULL == exn)
  {
    exn = caml_named_value("Hypertable.Error");
    assert(NULL != exn);
  }
  std::ostringstream os;
  os << e << endl;
  caml_raise_with_string(*exn, os.str().c_str());
}

CAML_HT_F4(client_create, v_install_dir, v_cfg_file, v_timeout_ms, v_unit)
{
  Client* p = NULL;
  String install_dir;
  uint32_t timeout = 0;

  if (Val_none != v_install_dir) install_dir = string_of_val(Some_val(v_install_dir));
  if (Val_none != v_timeout_ms) timeout = Int_val(Some_val(v_timeout_ms));

  if (Val_none == v_cfg_file)
  {
    p = new Client(install_dir, timeout);
  }
  else
  {
    p = new Client(install_dir, string_of_val(Some_val(v_cfg_file)), timeout);
  }
  CAMLreturn(wrapped<Client>::alloc(p));
}
CAML_HT_END

CAML_HT_F3(client_open_ns, v_client, v_base, v_name)
{
  Namespace* base = NULL;
  Namespace* ns = NULL;
  if (Val_none != v_base)
    base = wrapped<Namespace>::get(Some_val(v_base));
  /* FIXME wrap NamespacePtr */
  ns = wrapped<Client>::get(v_client)->open_namespace(string_of_val(v_name),base).get();
  CAMLreturn(wrapped<Namespace>::alloc(ns));
}
CAML_HT_END

CAML_HT_F3(ns_open_table, v_ns, v_name, v_force)
{
  /* FIXME wrap TablePtr */
  Table* p = wrapped<Namespace>::get(v_ns)->open_table(string_of_val(v_name), Bool_val(v_force)).get();
  CAMLreturn(wrapped<Table>::alloc(p));
}
CAML_HT_END

CAML_HT_F4(table_create_mutator, v_table, v_timeout, v_flags, v_flush_interval)
{
  TableMutator* p = wrapped<Table>::get(v_table)->create_mutator(Int_val(v_timeout), Int_val(v_flags), Int_val(v_flush_interval));
  CAMLreturn(wrapped<TableMutator>::alloc(p));
}
CAML_HT_END

CAML_HT_F4(table_create_scanner, v_table, v_scanspec, v_timeout, v_retry)
{
  ScanSpec& scanspec = wrapped<ScanSpecBuilder>::get(v_scanspec)->get();
  TableScanner* p = wrapped<Table>::get(v_table)->create_scanner(scanspec, Int_val(v_timeout), Bool_val(v_retry));
  CAMLreturn(wrapped<TableScanner>::alloc(p));
}
CAML_HT_END

CAML_HT_F1(scanspec_create, v_unit)
{
  CAMLreturn(wrapped<ScanSpecBuilder>::alloc(new ScanSpecBuilder()));
}
CAML_HT_END

CAML_HT_F2(scanspec_add_cf, v_ss, v_cf)
{
  wrapped<ScanSpecBuilder>::get(v_ss)->add_column(String_val(v_cf));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_add_row, v_ss, v_row)
{
  wrapped<ScanSpecBuilder>::get(v_ss)->add_row(String_val(v_row));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F4(scanspec_add_rows, v_ss, v_start, v_end, v_incl)
{
  wrapped<ScanSpecBuilder>::get(v_ss)->add_row_interval(
    String_val(v_start), Bool_val(v_incl),
    String_val(v_end), Bool_val(v_incl));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F1(tscan_next, t_scan)
{
  CAMLlocal2(v_cell, v_val);
  Cell cell;
  if (wrapped<TableScanner>::get(t_scan)->next(cell))
  {
    v_cell = caml_alloc(6, 0);
    v_val = caml_alloc_string(cell.value_len);
    memcpy(String_val(v_val), cell.value, cell.value_len);

    Store_field(v_cell,0,caml_copy_string(cell.row_key));
    Store_field(v_cell,1,caml_copy_string(cell.column_family));
    Store_field(v_cell,2,(NULL != cell.column_qualifier ? Val_some(caml_copy_string(cell.column_qualifier)) : Val_none));
    Store_field(v_cell,3,caml_copy_int64(cell.timestamp));
    Store_field(v_cell,4,v_val);
    Store_field(v_cell,5,Val_int(cell.flag));

    CAMLreturn(Val_some(v_cell));
  }
  else
  {
    CAMLreturn(Val_none);
  }
}
CAML_HT_END

CAML_HT_F3(tmut_set_key, v_tmut, v_key, v_val)
{
  KeySpec key;
  /* not copied */
  key.row = String_val(Field(v_key,0));
  key.row_len = caml_string_length(Field(v_key,0));
  key.column_family = String_val(Field(v_key,1));
  key.column_qualifier = String_val(Field(v_key,2));
  key.column_qualifier_len = caml_string_length(Field(v_key,2));
  key.timestamp = Int64_val(Field(v_key,3));
  wrapped<TableMutator>::get(v_tmut)->set(key, String_val(v_val), caml_string_length(v_val));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F5(tmut_set, v_tmut, v_row, v_cf, v_cq, v_val)
{
  KeySpec key;
  /* not copied */
  key.row = String_val(v_row);
  key.row_len = caml_string_length(v_row);
  key.column_family = String_val(v_cf);
  if (Val_none != v_cq) 
  {
    key.column_qualifier = String_val(Some_val(v_cq));
    key.column_qualifier_len = caml_string_length(Some_val(v_cq));
  }
  wrapped<TableMutator>::get(v_tmut)->set(key, String_val(v_val), caml_string_length(v_val));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F1(tmut_flush, v_tmut)
{
  wrapped<TableMutator>::get(v_tmut)->flush();
  CAMLreturn(Val_unit);
}
CAML_HT_END

} // extern "C"
