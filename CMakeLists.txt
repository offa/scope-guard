cmake_minimum_required(VERSION 3.2)

project(ScopeGuard VERSION 0.0.1)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

option(UNITTEST "Build Unit Tests" ON)


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


include_directories("include")


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

