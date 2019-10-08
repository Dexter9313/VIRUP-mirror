#!/bin/bash
if [ ! -f "./build.conf" ]; then
	. ./build.conf.example
else
	. ./build.conf
fi
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j "$@"
