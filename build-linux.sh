#!/bin/bash
. ./build.conf
mkdir -p build
cd build
cmake .. -DPROJECT_NAME=$PROJECT_NAME
make -j "$@"
