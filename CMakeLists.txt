cmake_minimum_required(VERSION 3.11)

project(ScopeGuard VERSION 0.3.8)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(UNITTEST "Build Unit Tests" ON)
option(ENABLE_COMPAT_HEADER "Enable compatible header 'scope'" OFF)
message(STATUS "Compatible Header : ${ENABLE_COMPAT_HEADER}")


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()



add_compile_options(-Wall
                    -Wextra
                    -pedantic
                    -pedantic-errors
                    -Werror
                    -Wshadow
                    -Wold-style-cast
                    -Wnull-dereference
                    -Wnon-virtual-dtor
                    -Woverloaded-virtual
                    )


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


include("${CMAKE_BINARY_DIR}/conan_paths.cmake" OPTIONAL)


add_library(ScopeGuard INTERFACE)
target_include_directories(ScopeGuard INTERFACE
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
                            )

if( ENABLE_COMPAT_HEADER )
    configure_file(cmake/scope.in generated/scope COPYONLY)

    target_include_directories(ScopeGuard INTERFACE
                                $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
                                $<INSTALL_INTERFACE:include>
                                )
endif()



if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

include(Install)
