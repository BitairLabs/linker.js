#!/bin/bash

linker=$1
config=$2

rm -rf build/$linker
rm -rf build/$linker.node

if [ -z $config ]; then
  cmake-js -d ./src/$linker -O build/$linker compile
  cp build/$linker/Release/$linker.node build/$linker.node
else
  cmake-js -D -d ./src/$linker -O build/$linker compile
  cp build/$linker/Debug/$linker.node build/$linker.node
fi
