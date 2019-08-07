#!/bin/bash
if [ ! -f "./build.conf" ]; then
	. ./build.conf.example
else
	. ./build.conf
fi
./build-linux.sh
if [ $? -ne 0 ]; then
	exit
fi
cd build
./$PROJECT_NAME
cd ..
