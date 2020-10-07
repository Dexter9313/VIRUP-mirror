#!/bin/bash

#to start profiling :
#callgrind_control -i on
#to stop profiling :
#callgrind_control -i off

if [ ! -f "./build.conf" ]; then
	. ./build.conf.example
else
	. ./build.conf
fi
export CMAKE_BUILD_TYPE=Debug
./build-linux.sh
if [ $? -ne 0 ]; then
	exit
fi
cd build
valgrind --tool=callgrind --instr-atstart=no ./$PROJECT_NAME
kcachegrind callgrind.*
rm callgrind.*
cd ..
