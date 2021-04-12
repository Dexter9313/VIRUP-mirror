#!/bin/bash

if [ ! -f "./build.conf" ]; then
	. ./build.conf.example
else
	. ./build.conf
fi
ls build/
apt-get update
dpkg -i build/*.deb || { apt-get -fy install; dpkg -i build/*.deb; }
ldd $(which $PROJECT_NAME)
$PROJECT_NAME --version
