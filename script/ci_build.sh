#!/bin/bash

set -ex

if [[ "${CXX}" == clang* ]]; then
    export CXXFLAGS="-stdlib=libc++"
    EXTRA="-DCMAKE_EXE_LINKER_FLAGS=-lc++abi -lc++"
fi

mkdir build && cd build

cmake ${EXTRA} ..
make
make unittest

