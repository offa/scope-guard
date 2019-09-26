#!/bin/bash

set -ex

# Conan
apt-get install -y python3-pip
pip3 install conan


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
    CONAN_STDLIB=libc++
else
    CONAN_STDLIB=libstdc++11
fi

mkdir build && cd build

conan install \
    -s compiler.libcxx=${CONAN_STDLIB} \
    -g cmake_find_package \
    -g cmake_paths \
    ..

cmake ..
make
make unittest

