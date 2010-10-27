// Simple template to wrap C++ object as OCaml custom value
// Copyright (C) 2010, ygrek <ygrek@autistici.org>
// 23/09/2010
//
// value wrapped<T>::alloc(T*)
//    creates custom value with C++ object inside
//    finalizer will call C++ destructor if object was not already destroyed
// void wrapped<T>::release(value)
//    calls destructor of wrapped object
// T* wrapped<T>::get(value)
//    returns pointer to wrapped object
//    raises OCaml Invalid_argument exception if object was already destroyed
//

extern "C" {
#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>
}

template<class T>
class wrapped
{
private:
  struct ml_wrapped
  {
    T* p;
  };

#define Wrapped_val(v) (*(ml_wrapped**)Data_custom_val(v))

  static void finalize(value v)
  {
    release(v);
    caml_stat_free(Wrapped_val(v));
  }

public:
  // name used as identifier for custom_operations
  // should be instantiated for each wrapped class
  static char const* ml_name();

  static T* get(value v)
  {
    CAMLparam1(v);
    T* p = Wrapped_val(v)->p;
    if (NULL == p) caml_invalid_argument(ml_name());
    CAMLreturnT(T*,p);
  }

  static void release(value v)
  {
    if (NULL == Wrapped_val(v)->p) return;
    delete Wrapped_val(v)->p;
    Wrapped_val(v)->p = NULL;
  }

  static value alloc(T* p)
  {
    CAMLparam0();
    CAMLlocal1(v);

    static struct custom_operations wrapped_ops = {
      const_cast<char*>(ml_name()),
      finalize,
      custom_compare_default,
      custom_hash_default,
      custom_serialize_default,
      custom_deserialize_default
    };

    v = caml_alloc_custom(&wrapped_ops, sizeof(ml_wrapped*), 0, 1);

    ml_wrapped* ml = (ml_wrapped*)caml_stat_alloc(sizeof(ml_wrapped));
    ml->p = p;
    Wrapped_val(v) = ml;

    CAMLreturn(v);
  }

#undef Wrapped_val

}; //wrapped

