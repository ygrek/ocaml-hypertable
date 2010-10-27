// Simple template to wrap C++ object as OCaml custom value
// Copyright (C) 2010, ygrek <ygrek@autistici.org>
// 27/10/2010
//
// value wrapped<Ptr>::alloc(Ptr)
//    creates custom value with pointer to C++ object inside
//    finalizer will release pointer (whether destructor will be called 
//    depends on the semantics of the pointer)
// void wrapped<Ptr>::release(value)
//    releases wrapped pointer
// Ptr const& wrapped<Ptr>::get(value)
//    returns pointer to wrapped object
//    raises OCaml Invalid_argument exception if pointer was already released
//
// wrapped<> manages smart pointers to C++ objects
// wrapped_ptr<> manages raw pointers (owns pointed object, release() destroys object)
//

extern "C" {
#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/custom.h>
#include <caml/fail.h>
}

#include <auto_ptr.h>

// name used as identifier for custom_operations
// should be instantiated for each wrapped pointer class
template<class T>
char const* ml_name();

// Ptr is a smart pointer class,
// e.g.: std::auto_ptr, boost::shared_ptr, boost::intrusive_ptr
template<class Ptr>
class wrapped
{
private:
  struct ml_wrapped
  {
    ml_wrapped(Ptr x) : p(x) {}
    Ptr p;
  };

#define Wrapped_val(v) (*(ml_wrapped**)Data_custom_val(v))

  static void finalize(value v)
  {
    release(v);
    caml_stat_free(Wrapped_val(v));
  }

public:
  typedef Ptr type;

  static Ptr const& get(value v) // do not copy
  {
    Ptr const& p = Wrapped_val(v)->p;
    //printf("get %lx : %s\n",(size_t)p.get(),ml_name<Ptr>());
    if (NULL == p.get()) caml_invalid_argument(ml_name<Ptr>());
    return p;
  }

  static void release(value v)
  {
    Ptr p = Wrapped_val(v)->p;
    //printf("release %lx : %s\n",(size_t)p.get(),ml_name<Ptr>());
    //if (NULL == p.get()) return;
    p.reset();
  }

  static value alloc(Ptr p) // copy is ok
  {
    //printf("alloc %lx : %s\n",(size_t)p.get(),ml_name<Ptr>());
    CAMLparam0();
    CAMLlocal1(v);

    static struct custom_operations wrapped_ops = {
      const_cast<char*>(ml_name<Ptr>()),
      finalize,
      custom_compare_default,
      custom_hash_default,
      custom_serialize_default,
      custom_deserialize_default
    };

    v = caml_alloc_custom(&wrapped_ops, sizeof(ml_wrapped*), 0, 1);

    ml_wrapped* ml = new ml_wrapped(p); //(ml_wrapped*)caml_stat_alloc(sizeof(ml_wrapped));
    Wrapped_val(v) = ml;

    CAMLreturn(v);
  }

#undef Wrapped_val

}; //wrapped

template<class T>
struct wrapped_ptr : public wrapped<std::auto_ptr<T> >
{
  typedef wrapped<std::auto_ptr<T> > base;
  static T* get(value v)
  {
    return base::get(v).get();
  }
  static value alloc(T* p)
  {
    return base::alloc(std::auto_ptr<T>(p));
  }
}; // wrapped_ptr

