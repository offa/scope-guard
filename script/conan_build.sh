#!/bin/bash

set -ex

export CONAN_ARCHS="x86_64"
export CONAN_BUILD_TYPES="Release"
export CONAN_CPPSTDS="17"
export CONAN_PURE_C="False"
export CONAN_BUILD_POLICY="missing"

pip install -U conan conan_package_tools
python build.py
