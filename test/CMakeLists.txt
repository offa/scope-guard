find_package(Catch2 REQUIRED)
find_package(trompeloeil REQUIRED)


function(add_test_suite name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE ScopeGuard Catch2::Catch2WithMain trompeloeil::trompeloeil)
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

