#!/bin/bash

apt-get update ; apt-get install -y clang-format-7 clang-tidy-7
mkdir -p build ; cd build
cmake ..
make clang-format
make clang-tidy
