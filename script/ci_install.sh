#!/bin/bash

BUILD_DIR=${TRAVIS_BUILD_DIR}

## Install CMake
mkdir -p "${DEPENDENCY_DIR}" && cd "${DEPENDENCY_DIR}"

if [[ ! -d "$(ls -A ${DEPENDENCY_DIR}/cmake)" ]]; then
    CMAKE_URL="https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.tar.gz"
    mkdir -p cmake
    wget --no-check-certificate --quiet -O - "${CMAKE_URL}" | tar --strip-components=1 -xz -C cmake
fi

cd ${BUILD_DIR}

