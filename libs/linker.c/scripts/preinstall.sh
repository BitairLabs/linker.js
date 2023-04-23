#!/bin/sh

cd ./deps

rm -rf build
mkdir build

tar -xvzf libffi.tar.gz -C build

cd ./build/libffi
sh ./autogen.sh
build_dir="$PWD/"
sh configure --with-pic --disable-shared --disable-docs --prefix $build_dir
make
make install
