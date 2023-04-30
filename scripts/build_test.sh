#!/bin/sh

cd build
rm -rf sample_libs
mkdir sample_libs && cd sample_libs

mkdir c && mkdir cpp && mkdir go && mkdir rust

cd c
gcc -c -fPIC ../../../test/c_linker/sample_libs/c/lib.c && gcc -shared -o lib.so lib.o
cd ..

cd cpp
gcc -c -fPIC ../../../test/c_linker/sample_libs/cpp/lib.cpp && gcc -shared -o lib.so lib.o
cd ..

cd go
go build -o lib.so -buildmode=c-shared ../../../test/c_linker/sample_libs/go/lib.go
cd ..

cd ../../test/c_linker/sample_libs/rust
cargo build --release --target-dir ../../../../build/sample_libs/rust
