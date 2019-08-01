#!/bin/bash
if [ ! -f "./build.conf" ]; then
	. ./build.conf.example
else
	. ./build.conf
fi
./build-linux.sh
cd build
./$PROJECT_NAME
cd ..
