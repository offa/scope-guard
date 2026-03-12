#!/bin/bash

set -ex

conan create -o unittest=True --build=missing .
