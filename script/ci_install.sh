#!/bin/bash

set -ex

BUILD_DIR=${TRAVIS_BUILD_DIR}

mkdir -p "${DEPENDENCY_DIR}" && cd "${DEPENDENCY_DIR}"


# --- CMake
CMAKE_INSTALLER=install-cmake.sh

if [[ ! -f ${CMAKE_INSTALLER} ]]
then
    curl -sSL https://cmake.org/files/v3.8/cmake-3.8.2-Linux-x86_64.sh -o ${CMAKE_INSTALLER}
    chmod +x ${CMAKE_INSTALLER}
fi

sudo ./${CMAKE_INSTALLER} --prefix=/usr/local --skip-license
cmake --version


cd ${DEPENDENCY_DIR}


# --- LibC++
if [[ "${CXX}" = clang* ]]
then
    if [[ ! -d "${DEPENDENCY_DIR}/llvm-source" ]]
    then
        LLVM_RELEASE=release_50
        git clone --depth=1 -b ${LLVM_RELEASE} https://github.com/llvm-mirror/llvm.git llvm-source
        git clone --depth=1 -b ${LLVM_RELEASE} https://github.com/llvm-mirror/libcxx.git llvm-source/projects/libcxx
        git clone --depth=1 -b ${LLVM_RELEASE} https://github.com/llvm-mirror/libcxxabi.git llvm-source/projects/libcxxabi
    fi

    mkdir -p build && cd build

    cmake -DCMAKE_C_COMPILER=${CC} \
        -DCMAKE_CXX_COMPILER=${CXX} \
        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
        -DCMAKE_INSTALL_PREFIX=/usr \
        -DLIBCXX_ABI_UNSTABLE=ON \
        ../llvm-source
    make cxx -j4

    sudo make install-cxxabi install-cxx
fi


cd ${BUILD_DIR}
