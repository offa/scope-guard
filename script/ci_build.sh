#!/bin/bash

set -ex

BUILD_TYPE="Release"

# Conan
export DEBIAN_FRONTEND=noninteractive
export PATH=$HOME/.local/bin:$PATH
apt-get update
apt-get install -y pipx
pipx install conan
conan profile detect

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
    sed -i 's/^compiler.libcxx=.*$/compiler.libcxx=libc++/g' ~/.conan2/profiles/default
fi

mkdir build && cd build

conan install \
    --build=missing \
    -of . \
    -s compiler.cppstd=17 \
    -s build_type=${BUILD_TYPE} \
    ..

cmake -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ..
make
make unittest
