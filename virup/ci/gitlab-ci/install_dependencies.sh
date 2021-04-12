#!/bin/bash

cd deps
git clone https://gitlab.com/Dexter9313/octree-file-format.git ;
cd octree-file-format/liboctree ;
mkdir build ; cd build
cmake ..
make package -j
dpkg -i ./*.deb ;
cd ../../.. ;
apt-get install -y libboost-dev ;
cd ..
