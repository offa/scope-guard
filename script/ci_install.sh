#!/bin/bash

set -ex

BUILD_DIR=${TRAVIS_BUILD_DIR}

mkdir -p "${DEPENDENCY_DIR}" && cd "${DEPENDENCY_DIR}"


## Install CMake
CMAKE_INSTALLER=install-cmake.sh

if [[ ! -f ${CMAKE_INSTALLER} ]]
then
    curl -sSL https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.sh -o ${CMAKE_INSTALLER}
    chmod +x ${CMAKE_INSTALLER}
fi

sudo ./${CMAKE_INSTALLER} --prefix=/usr/local --skip-license
cmake --version


cd ${BUILD_DIR}

