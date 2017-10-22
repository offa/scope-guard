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
#if [[ "${CXX}" = clang* ]]
#then
#    if [[ ! -d "${DEPENDENCY_DIR}/llvm-source" ]]
#    then
#        LLVM_RELEASE=release_50
#        git clone --depth=1 -b ${LLVM_RELEASE} https://github.com/llvm-mirror/llvm.git llvm-source
#        git clone --depth=1 -b ${LLVM_RELEASE} https://github.com/llvm-mirror/libcxx.git llvm-source/projects/libcxx
#        git clone --depth=1 -b ${LLVM_RELEASE} https://github.com/llvm-mirror/libcxxabi.git llvm-source/projects/libcxxabi
#    fi
#
#    mkdir -p build && cd build
#
#    cmake -DCMAKE_C_COMPILER=${CC} \
#        -DCMAKE_CXX_COMPILER=${CXX} \
#        -DCMAKE_BUILD_TYPE=RelWithDebInfo \
#        -DCMAKE_INSTALL_PREFIX=/usr \
#        -DLIBCXX_ABI_UNSTABLE=ON \
#        ../llvm-source
#    make cxx -j4
#
#    sudo make install-cxxabi install-cxx
#    rm -rf *
#fi

# source: https://github.com/ericniebler/range-v3

TRUNK_VERSION="5.*"

set -e

# The pattern of clang --version is: clang version X.Y.Z (sometimes, see below).
COMPILER_VERSION_OUTPUT="$($CXX --version)"
arr=(${COMPILER_VERSION_OUTPUT// / })

COMPILER="${arr[0]}"
VERSION="${arr[2]}"

case $COMPILER in
    "clang")
        # Some Ubuntu clang builds are advertised as "just clang", but the
        # Version still follows the pattern: 3.6.2-svn240577-1~exp1
        # echo "Compiler is clang :)"
        arr2=(${VERSION//-/ })
        VERSION="${arr2[0]}"
        ;;
    "Ubuntu")
        # Ubuntu renames _some_ (not all) of its clang compilers, the pattern of
        # clang --version is then:
        # Ubuntu clang version 3.6.2-svn240577-1~exp1
        COMPILER="${arr[1]}"
        VERSION="${arr[3]}"
        arr2=(${VERSION//-/ })
        VERSION="${arr2[0]}"
        ;;
    *)
        echo "case did not match: compiler: ${COMPILER}"
        exit 1
        ;;
esac

if [ ${COMPILER} != "clang" ]; then
    echo "Error: trying to install libc++ for a compiler that is not clang: ${COMPILER}"
    exit 1
fi

if [ -z ${VERSION+x} ]; then
    echo "libc++ version is not set. To set the libc++ version: ./install_libcxx.sh -v X.Y.Z"
    exit 4
fi

if [[ ${VERSION} == $TRUNK_VERSION ]]; then
    echo "Fetching libc++ and libc++abi tip-of-trunk..."

    # Checkout LLVM sources
    git clone --depth=1 https://github.com/llvm-mirror/llvm.git llvm-source
    git clone --depth=1 https://github.com/llvm-mirror/libcxx.git llvm-source/projects/libcxx
    git clone --depth=1 https://github.com/llvm-mirror/libcxxabi.git llvm-source/projects/libcxxabi
else
    echo "Fetching libc++/libc++abi version: ${VERSION}..."
    LLVM_URL="http://releases.llvm.org/${VERSION}/llvm-${VERSION}.src.tar.xz"
    LIBCXX_URL="http://releases.llvm.org/${VERSION}/libcxx-${VERSION}.src.tar.xz"
    LIBCXXABI_URL="http://releases.llvm.org/${VERSION}/libcxxabi-${VERSION}.src.tar.xz"
    curl -O $LLVM_URL
    curl -O $LIBCXX_URL
    curl -O $LIBCXXABI_URL

    mkdir llvm-source
    mkdir llvm-source/projects
    mkdir llvm-source/projects/libcxx
    mkdir llvm-source/projects/libcxxabi

    tar -xf llvm-${VERSION}.src.tar.xz -C llvm-source --strip-components=1
    tar -xf libcxx-${VERSION}.src.tar.xz -C llvm-source/projects/libcxx --strip-components=1
    tar -xf libcxxabi-${VERSION}.src.tar.xz -C llvm-source/projects/libcxxabi --strip-components=1
fi

mkdir llvm-build
cd llvm-build

# - libc++ versions < 4.x do not have the install-cxxabi and install-cxx targets
# - only ASAN is enabled for clang/libc++ versions < 4.x
if [[ $VERSION == *"3."* ]]; then
    cmake -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} \
          -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr \
          ../llvm-source
    if [[ $SANITIZER == "Address;Undefined" ]]; then
        ASAN_FLAGS="-fsanitize=address"
        cmake -DCMAKE_CXX_FLAGS="${ASAN_FLAGS}" -DCMAKE_EXE_LINKER_FLAGS="${ASAN_FLAGS}" ../llvm-source
    fi
    make cxx -j2 VERBOSE=0
    sudo cp -r lib/* /usr/lib/
    sudo cp -r include/c++ /usr/include/
else
    cmake -DCMAKE_C_COMPILER=${CC} -DCMAKE_CXX_COMPILER=${CXX} \
          -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr \
          -DLIBCXX_ABI_UNSTABLE=ON \
          -DLLVM_USE_SANITIZER=${SANITIZER} \
          ../llvm-source
    make cxx -j2 VERBOSE=0
    sudo make install-cxxabi install-cxx
fi




cd ${BUILD_DIR}
