#!/bin/bash

set -ex

# Conan
apt-get install -y python3-pip
pip3 install conan
conan profile new default --detect

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
else
    conan profile update settings.compiler.libcxx=libstdc++11 default
fi

mkdir build && cd build

conan install \
    -g cmake_find_package \
    -g cmake_paths \
    ..

cmake -DCMAKE_BUILD_TYPE=Release ..
make
make unittest
