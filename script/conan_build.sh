#!/bin/bash

set -ex

pip install -U conan conan_package_tools
python build.py
