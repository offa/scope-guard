#!/bin/bash

set -ex

export CONAN_DOCKER_IMAGE=conanio/gcc${CONAN_GCC_VERSIONS}

pip install -U conan conan_package_tools
python build.py
