#!/bin/bash

set -ex

apt-get update
apt-get install -y pipx
pipx install conan

conan profile detect
conan create --build=missing .
