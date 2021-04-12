#!/bin/bash

mkdir build ; cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DWERROR=true
export VERSION=$(cat PROJECT_VERSION)
make -j 8
make package
./tests

