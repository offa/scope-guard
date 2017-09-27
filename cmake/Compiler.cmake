include(CheckCXXCompilerFlag)

function(add_compile_options_if_supported)
    foreach(flag ${ARGN})
        set(option_var option_supported_${flag})
        CHECK_CXX_COMPILER_FLAG(${flag} ${option_var})

        if( ${option_var} )
            add_compile_options(${flag})
        endif()
    endforeach()
endfunction()

