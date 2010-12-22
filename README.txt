
ocaml-hypertable
================

OCaml bindings to Hypertable <http://hypertable.org> C++ library.

Build
-----

Install hypertable and all the dependencies.
Build and install this library :

  make all install

Known problems
--------------

The path to C++ library is hardcoded
to `/opt/hypertable/current`, make it
a symlink to your actual installation or edit
_oasis and regenerate build system with `oasis setup`.

Linking error with libHypertable.a prevents building dynamic stublibs, hence
-failsafe option used. But oasis expects to find dllhypertable.so among build results,
so the check is suppressed in setup.ml (function check_and_register).
So setup.ml needs manual editing after regeneration.

