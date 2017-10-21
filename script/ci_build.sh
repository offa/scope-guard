#!/bin/bash

set -ex

if [[ "${CXX}" == clang* ]]; then
    export CXXFLAGS="-stdlib=libc++ -lc++abi"
fi

mkdir build && cd build

cmake ..
make
make unittest

