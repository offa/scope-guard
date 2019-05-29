#!/bin/bash

set -ex

# Conan
apt-get install -y python3-pip
pip3 install conan


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi

mkdir build && cd build
conan install --build missing -s compiler.libcxx=libstdc++11 ..

cmake ..
make
make unittest

