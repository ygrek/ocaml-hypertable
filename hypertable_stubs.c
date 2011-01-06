
#include "cxx_wrapped.h"
#include <Common/Compat.h>
#include <Hypertable/Lib/Client.h>

using namespace Hypertable;

typedef wrapped<NamespacePtr> ml_Namespace;
typedef wrapped<ClientPtr> ml_Client;
typedef wrapped<TablePtr> ml_Table;
typedef wrapped<TableMutatorPtr> ml_TableMutator;
typedef wrapped<TableScannerPtr> ml_TableScanner;
typedef wrapped_ptr<ScanSpecBuilder> ml_ScanSpecBuilder;

template<> char const* ml_name<ml_Client::type>() { return "hypertable.Client"; }
template<> char const* ml_name<ml_Namespace::type>() { return "hypertable.Namespace"; }
template<> char const* ml_name<ml_Table::type>() { return "hypertable.Table"; }
template<> char const* ml_name<ml_TableMutator::type>() { return "hypertable.TableMutator"; }
template<> char const* ml_name<ml_TableScanner::type>() { return "hypertable.TableScanner"; }
template<> char const* ml_name<ml_ScanSpecBuilder::type>() { return "hypertable.ScanSpecBuilder"; }

extern "C" {

#include <caml/callback.h>

#define Val_none Val_int(0)

static value Val_some(value v)
{
    CAMLparam1(v);
    CAMLlocal1(some);
    some = caml_alloc_small(1, 0);
    Field(some, 0) = v;
    CAMLreturn(some);
}

static value Val_pair(value v1, value v2)
{
  CAMLparam2(v1,v2);
  CAMLlocal1(pair);
  pair = caml_alloc_small(2,0);
  Field(pair,0) = v1;
  Field(pair,1) = v2;
  CAMLreturn(pair);
}

static value Val_cons(value list, value v) { return Val_pair(v,list); }

#define Val_nil Val_int(0)

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

#define CAML_HT_DTOR(cls,name) \
CAML_HT_F1(name, v) \
{ \
  cls::release(v); \
  CAMLreturn(Val_unit); \
} \
CAML_HT_END

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

CAMLprim value caml_hypertable_values_count(value v_unit)
{
  CAMLparam1(v_unit);
  CAMLlocal1(v_list);

  v_list = Val_nil;
  v_list = Val_cons(v_list, Val_pair(caml_copy_string(ml_Table::name()), Val_int(ml_Table::count())));
  v_list = Val_cons(v_list, Val_pair(caml_copy_string(ml_TableScanner::name()), Val_int(ml_TableScanner::count())));
  v_list = Val_cons(v_list, Val_pair(caml_copy_string(ml_TableMutator::name()), Val_int(ml_TableMutator::count())));
  v_list = Val_cons(v_list, Val_pair(caml_copy_string(ml_Namespace::name()), Val_int(ml_Namespace::count())));
  v_list = Val_cons(v_list, Val_pair(caml_copy_string(ml_Client::name()), Val_int(ml_Client::count())));
  v_list = Val_cons(v_list, Val_pair(caml_copy_string(ml_ScanSpecBuilder::name()), Val_int(ml_ScanSpecBuilder::count())));

  CAMLreturn(v_list);
}

CAML_HT_F4(client_create, v_install_dir, v_cfg_file, v_timeout_ms, v_unit)
{
  ClientPtr p;
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
  CAMLreturn(ml_Client::alloc(p));
}
CAML_HT_END

CAML_HT_F3(client_open_ns, v_client, v_base, v_name)
{
  NamespacePtr base;
  if (Val_none != v_base)
    base = ml_Namespace::get(Some_val(v_base));
  NamespacePtr ns = ml_Client::get(v_client)->open_namespace(string_of_val(v_name),base.get());
  CAMLreturn(ml_Namespace::alloc(ns));
}
CAML_HT_END

CAML_HT_F4(client_create_ns, v_client, v_base, v_create_intermediate, v_name)
{
  NamespacePtr base;
  bool create_intermediate = false;
  if (Val_none != v_base)
    base = ml_Namespace::get(Some_val(v_base));
  if(Val_none != v_create_intermediate)
    create_intermediate = Bool_val(Some_val(v_create_intermediate));
  ml_Client::get(v_client)->create_namespace(string_of_val(v_name), base.get(), create_intermediate);
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F3(client_exists_ns, v_client, v_base, v_name)
{
  NamespacePtr base;
  if (Val_none != v_base)
    base = ml_Namespace::get(Some_val(v_base));
  int exists = ml_Client::get(v_client)->exists_namespace(string_of_val(v_name),base.get());
  CAMLreturn(Bool_val(exists));
}
CAML_HT_END

CAML_HT_F4(client_drop_ns, v_client, v_base, v_if_exists, v_name)
{
  NamespacePtr base;
  bool if_exists = false;
  if (Val_none != v_base)
    base = ml_Namespace::get(Some_val(v_base));
  if(Val_none != v_if_exists)
    if_exists = Bool_val(Some_val(v_if_exists));
  ml_Client::get(v_client)->drop_namespace(string_of_val(v_name), base.get(), if_exists);
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F3(ns_open_table, v_ns, v_name, v_force)
{
  TablePtr p = ml_Namespace::get(v_ns)->open_table(string_of_val(v_name), Bool_val(v_force));
  CAMLreturn(ml_Table::alloc(p));
}
CAML_HT_END

CAML_HT_F3(ns_create_table, v_ns, v_name, v_schema)
{
  ml_Namespace::get(v_ns)->create_table(string_of_val(v_name), string_of_val(v_schema));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F3(ns_alter_table, v_ns, v_name, v_schema)
{
  ml_Namespace::get(v_ns)->alter_table(string_of_val(v_name), string_of_val(v_schema));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F3(ns_drop_table, v_ns, v_name, v_if_exists)
{
  ml_Namespace::get(v_ns)->drop_table(string_of_val(v_name), Bool_val(v_if_exists));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F3(ns_rename_table, v_ns, v_old_name, v_new_name)
{
  ml_Namespace::get(v_ns)->rename_table(string_of_val(v_old_name), string_of_val(v_new_name));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(ns_refresh_table, v_ns, v_name)
{
  ml_Namespace::get(v_ns)->refresh_table(string_of_val(v_name));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(ns_exists_table, v_ns, v_name)
{
  int exists = ml_Namespace::get(v_ns)->exists_table(string_of_val(v_name));
  CAMLreturn(Bool_val(exists));
}
CAML_HT_END

CAML_HT_F3(ns_get_schema, v_ns, v_with_ids, v_name)
{
  CAMLlocal1(v_schema);
  String schema = ml_Namespace::get(v_ns)->get_schema_str(string_of_val(v_name), Bool_val(v_with_ids));
  v_schema = caml_alloc_string(schema.length());
  CAMLreturn(v_schema);
}
CAML_HT_END

CAML_HT_F4(table_create_mutator, v_table, v_timeout, v_flags, v_flush_interval)
{
  TableMutatorPtr p = ml_Table::get(v_table)->create_mutator(Int_val(v_timeout), Int_val(v_flags), Int_val(v_flush_interval));
  CAMLreturn(ml_TableMutator::alloc(p));
}
CAML_HT_END

CAML_HT_F4(table_create_scanner, v_table, v_scanspec, v_timeout, v_retry)
{
  ScanSpec& scanspec = ml_ScanSpecBuilder::get(v_scanspec)->get();
  TableScannerPtr p = ml_Table::get(v_table)->create_scanner(scanspec, Int_val(v_timeout), Bool_val(v_retry));
  CAMLreturn(ml_TableScanner::alloc(p));
}
CAML_HT_END

CAML_HT_F1(scanspec_create, v_unit)
{
  CAMLreturn(ml_ScanSpecBuilder::alloc(new ScanSpecBuilder()));
}
CAML_HT_END

CAML_HT_F1(scanspec_reset, v_ss)
{
  ml_ScanSpecBuilder::get(v_ss)->clear();
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_add_cf, v_ss, v_cf)
{
  ml_ScanSpecBuilder::get(v_ss)->add_column(String_val(v_cf));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_add_row, v_ss, v_row)
{
  ml_ScanSpecBuilder::get(v_ss)->add_row(String_val(v_row));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F4(scanspec_add_rows, v_ss, v_start, v_end, v_incl)
{
  ml_ScanSpecBuilder::get(v_ss)->add_row_interval(
    String_val(v_start), Bool_val(v_incl),
    String_val(v_end), Bool_val(v_incl));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F4(scanspec_add_cells, v_ss, v_start, v_end, v_incl)
{
  ml_ScanSpecBuilder::get(v_ss)->add_cell_interval(
    String_val(Field(v_start,0)), String_val(Field(v_start,1)), Bool_val(v_incl),
    String_val(Field(v_end,0)), String_val(Field(v_end,1)), Bool_val(v_incl));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_keys_only, v_ss, v_b)
{
  ml_ScanSpecBuilder::get(v_ss)->set_keys_only(Bool_val(v_b));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_set_cell_limit, v_ss, v_n)
{
  ml_ScanSpecBuilder::get(v_ss)->set_cell_limit(Int_val(v_n));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_set_row_limit, v_ss, v_n)
{
  ml_ScanSpecBuilder::get(v_ss)->set_row_limit(Int_val(v_n));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(scanspec_set_max_versions, v_ss, v_n)
{
  ml_ScanSpecBuilder::get(v_ss)->set_max_versions(Int_val(v_n));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F1(tscan_next, t_scan)
{
  CAMLlocal2(v_cell, v_val);
  Cell cell;
  if (ml_TableScanner::get(t_scan)->next(cell))
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

CAML_HT_F1(tscan_bytes, v_tscan)
{
  CAMLreturn(caml_copy_int64(ml_TableScanner::get(v_tscan)->bytes_scanned()));
}
CAML_HT_END

#define CAML_UNLOCKED(smth) do { caml_blocking_section lock; smth; } while (0)

CAML_HT_F3(tmut_set_key, v_tmut, v_key, v_val)
{
  KeySpecBuilder key;
  key.set_row(String_val(Field(v_key,0)));
  key.set_column_family(String_val(Field(v_key,1)));
  if (Val_none != Field(v_key,2)) 
  {
    key.set_column_qualifier(String_val(Some_val(Field(v_key,2))));
  }
  key.set_timestamp(Int64_val(Field(v_key,3)));
  String val(String_val(v_val), caml_string_length(v_val));
  TableMutatorPtr p = ml_TableMutator::get(v_tmut);
  CAML_UNLOCKED(p->set(key.get(), val));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F5(tmut_set, v_tmut, v_row, v_cf, v_cq, v_val)
{
  KeySpecBuilder key;
  key.set_row(String_val(v_row));
  key.set_column_family(String_val(v_cf));
  if (Val_none != v_cq) 
  {
    key.set_column_qualifier(String_val(Some_val(v_cq)));
  }
  String val(String_val(v_val), caml_string_length(v_val));
  TableMutatorPtr p = ml_TableMutator::get(v_tmut);
  CAML_UNLOCKED(p->set(key.get(), val));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F2(tmut_set_delete, v_tmut, v_key)
{
  KeySpecBuilder key;
  key.set_row(String_val(Field(v_key,0)));
  if (Val_none != Field(v_key,1)) 
  {
    key.set_column_family(String_val(Field(v_key,1)));
  }
  if (Val_none != Field(v_key,2)) 
  {
    key.set_column_qualifier(String_val(Some_val(Field(v_key,2))));
  }
  key.set_timestamp(Int64_val(Field(v_key,3)));
  TableMutatorPtr p = ml_TableMutator::get(v_tmut);
  CAML_UNLOCKED(p->set_delete(key.get()));
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F1(tmut_flush, v_tmut)
{
  TableMutatorPtr p = ml_TableMutator::get(v_tmut);
  CAML_UNLOCKED(p->flush());
  CAMLreturn(Val_unit);
}
CAML_HT_END

CAML_HT_F1(tmut_memory_used, v_tmut)
{
  TableMutatorPtr p = ml_TableMutator::get(v_tmut);
  uint64_t mem;
  CAML_UNLOCKED(mem = p->memory_used());
  CAMLreturn(caml_copy_int64(mem));
}
CAML_HT_END

CAML_HT_DTOR(ml_ScanSpecBuilder,scanspec_release)
CAML_HT_DTOR(ml_TableScanner,tscan_release)
CAML_HT_DTOR(ml_TableMutator,tmut_release)
CAML_HT_DTOR(ml_Table,table_release)
CAML_HT_DTOR(ml_Client,client_release)
CAML_HT_DTOR(ml_Namespace,ns_release)

} // extern "C"

