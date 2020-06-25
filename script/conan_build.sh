#!/bin/bash

set -ex

export CONAN_DOCKER_IMAGE=conanio/gcc${CONAN_GCC_VERSIONS}
export CONAN_ARCHS="x86_64"
export CONAN_BUILD_TYPES=Release

pip install -U conan conan_package_tools
python build.py
