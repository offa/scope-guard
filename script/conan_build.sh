#!/bin/bash

set -ex

export PATH=$HOME/.local/bin:$PATH
apt-get update
apt-get install -y pipx
pipx install conan

conan profile detect
conan create -o unittest=True --build=missing .
