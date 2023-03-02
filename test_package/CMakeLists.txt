cmake_minimum_required(VERSION 3.11)
project(test_package CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ScopeGuard REQUIRED)

add_executable(test_package example.cpp)
target_link_libraries(test_package PRIVATE ScopeGuard::ScopeGuard)

