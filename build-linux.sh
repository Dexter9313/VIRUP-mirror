#!/bin/bash
. ./build.conf
mkdir -p build
cd build
cmake ..
make -j "$@"
