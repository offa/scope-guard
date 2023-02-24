#!/bin/bash

set -ex

apt-get update
apt-get install -y python3-pip
pip3 install -U conan

conan profile detect
conan create --build=missing .
