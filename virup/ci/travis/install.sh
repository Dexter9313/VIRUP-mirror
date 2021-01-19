#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	git clone https://gitlab.com/Dexter9313/octree-file-format.git ;
	cd octree-file-format/liboctree ;
	mkdir build
	cd build
	cmake ..
	make package -j
	sudo dpkg -i ./*.deb ;
	cd ../../.. ;
	sudo apt-get install libboost-dev ;
fi
