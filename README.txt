
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

Here is the error that prevents building dynamic stublibs (it can be ignored as long
as native code library and example program are built successfully) :

+ /usr/bin/ocamlmklib -o hypertable -L/opt/hypertable/current/lib -failsafe -lstdc++ -lHypertable -lHyperComm -lHyperDfsBroker -lHyperCommon -lHyperspace -lHyperTools -llog4cpp -lexpat -lboost_thread -lboost_iostreams -lboost_program_options -lboost_system -lsigar-amd64-linux -lz -lcurses -lrrd -lboost_filesystem hypertable_stubs.o
/usr/bin/ld: /opt/hypertable/current/lib/libHypertable.a(Client.cc.o): relocation R_X86_64_32 against `.bss' can not be used when making a shared object; recompile with -fPIC
/opt/hypertable/current/lib/libHypertable.a: could not read symbols: Bad value
collect2: ld returned 1 exit status
Finished, 11 targets (0 cached) in 00:00:18.
W: Cannot find an existing alternative files among: _build/dllhypertable.so

