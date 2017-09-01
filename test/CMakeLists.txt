
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE "catch")


add_library(TestMain TestMain.cpp)
target_link_libraries(TestMain Catch)


function(add_test_suite name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} TestMain ScopeGuard)
    add_test(NAME ${name} COMMAND ${name})
endfunction()


add_test_suite(ScopeGuardTest)



add_custom_target(unittest ScopeGuardTest
                    COMMENT "Running unittests\n\n"
                    VERBATIM
                    )


