#!/bin/bash
if [ ! -f "./build.conf" ]; then
    cp ./build.conf.example ./build.conf
fi
. ./build.conf
mkdir -p build
cd build
cmake ..
make -j "$@"
