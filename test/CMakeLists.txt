find_package(Catch2 REQUIRED)

add_library(Trompeloeil INTERFACE)
target_include_directories(Trompeloeil INTERFACE "trompeloeil")


add_library(TestMain TestMain.cpp)
target_link_libraries(TestMain PUBLIC Catch2::Catch2 Trompeloeil)


function(add_test_suite name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE TestMain ScopeGuard)
    add_test(${name} ${name})
endfunction()


add_test_suite(ScopeExitTest)
add_test_suite(ScopeSuccessTest)
add_test_suite(ScopeFailTest)
add_test_suite(UniqueResourceTest)


add_custom_target(unittest ScopeExitTest
                    COMMAND ScopeSuccessTest
                    COMMAND ScopeFailTest
                    COMMAND UniqueResourceTest
                    COMMENT "Running unittests\n\n"
                    VERBATIM
                    )

