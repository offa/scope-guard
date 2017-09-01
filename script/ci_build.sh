#!/bin/bash

if [[ "$CXX" == clang* ]]; then
    export CXXFLAGS="-stdlib=libc++"
fi

mkdir build && cd build

cmake ..
make
make unittest

