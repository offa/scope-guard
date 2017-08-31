
add_library(testmain TestMain.cpp)
target_include_directories(testmain SYSTEM PUBLIC "catch/")

function(add_test_suite name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} testmain)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_test_suite(ScopeGuardTest)




add_custom_target(unittest ScopeGuardTest
                    COMMENT "Running unittests\n\n"
                    VERBATIM
                    )


