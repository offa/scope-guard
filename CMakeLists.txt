cmake_minimum_required(VERSION 3.2)

project(ScopeGuard VERSION 0.1.0)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(UNITTEST "Build Unit Tests" ON)
option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Coverage : ${COVERAGE}")
option(ENABLE_COMPAT_HEADER "Enable compatible header 'scope'" OFF)
message(STATUS "Compatible Header : ${ENABLE_COMPAT_HEADER}")


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()


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


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)




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


