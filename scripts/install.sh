#!/bin/bash

command=$1
linker=$2

root_dir=node_modules/@bitair/linker.js
src_dir=$root_dir/src
build_dir=$root_dir/build

rm -rf $build_dir/$linker
rm -rf $build_dir/$linker.node

if [ $command == "install" ]; then
  npx cmake-js -d $src_dir/$linker -O $build_dir/$linker compile
  cp $build_dir/$linker/Release/$linker.node $root_dir/build/$linker.node
else
  echo "Linker.js Error: Uknown command $command"
fi
