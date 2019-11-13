#!/bin/bash
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
gdb -ex run --args ./$PROJECT_NAME
cd ..
