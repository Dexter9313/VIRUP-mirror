#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	wget --content-disposition https://gitlab.com/Dexter9313/octree-file-format/-/jobs/artifacts/1.5.0/raw/liboctree-1.5.0-linux_amd64.deb?job=pack:liboctree ;
	sudo dpkg -i ./*.deb ;
fi
