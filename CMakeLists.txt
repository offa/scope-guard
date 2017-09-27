cmake_minimum_required(VERSION 3.2)

project(ScopeGuard VERSION 0.0.3)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(UNITTEST "Build Unit Tests" ON)
option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Coverage : ${COVERAGE}")


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()

include(Compiler)

if( COVERAGE )
    include(Coverage)
endif()


add_compile_options(-Wall
                    -Wextra
                    -pedantic
                    -Werror
                    -Wshadow
                    -Wold-style-cast
                    )
add_compile_options_if_supported(-Wduplicated-cond
                                -Wduplicated-branches
                                -Wlogical-op
                                -Wnull-dereference
                                -Wuseless-cast
                                )


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_library(ScopeGuard INTERFACE)
target_include_directories(ScopeGuard INTERFACE
                       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                       $<INSTALL_INTERFACE:include>)


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

include(Install)


