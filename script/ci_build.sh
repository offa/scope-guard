#!/bin/bash

set -ex

# Conan
apt-get install -y python3-pip
pip3 install conan

# #156 Workaround for conan#4938
mkdir -p ~/.conan
cp script/settings.yml ~/.conan/


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
    CONAN_STDLIB=libc++
else
    CONAN_STDLIB=libstdc++11
fi

mkdir build && cd build
conan install --build missing -s compiler.libcxx=${CONAN_STDLIB} ..

cmake ..
make
make unittest

