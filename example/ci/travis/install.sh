#!/bin/bash

# install boost
echo "$TRAVIS_OS_NAME"
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
	sudo apt-get install libboost-dev
fi

