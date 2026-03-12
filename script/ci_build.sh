#!/bin/bash

set -ex

BUILD_TYPE="Release"

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
    sed -i 's/^compiler.libcxx=.*$/compiler.libcxx=libc++/g' ~/.conan2/profiles/default
fi

mkdir build && cd build

conan install \
    -o unittest=True \
    --build=missing \
    -of . \
    -s compiler.cppstd=17 \
    -s build_type=${BUILD_TYPE} \
    ..

cmake -DCMAKE_TOOLCHAIN_FILE=./conan_toolchain.cmake -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" ..
make
make unittest
