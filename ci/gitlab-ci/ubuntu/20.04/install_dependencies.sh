#/bin/bash

./ci/gitlab-ci/commons/install_dependencies.sh
cd deps;
git clone https://github.com/Orochimarufan/PythonQt ;
cd PythonQt ;
git checkout 18d4c249ca9b0003cfb10ad711c60fb7f9d5f79b ;
sed -i "s/PythonQt_init_QtQml(0);//" ./extensions/PythonQt_QtAll/PythonQt_QtAll.cpp ;
sed -i "s/PythonQt_init_QtQuick(0);//" ./extensions/PythonQt_QtAll/PythonQt_QtAll.cpp ;
cd build ;
cmake .. -DBUILD_SHARED_LIBS=ON ;
make -j 8 ;
make install ;
cd ../../..
# install openvr
apt-get install -y libopenvr-dev
rm -rf /deps
