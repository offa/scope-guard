
install(TARGETS ScopeGuard EXPORT ScopeGuard-config DESTINATION include)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)

if( ENABLE_COMPAT_HEADER )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/" DESTINATION include)
endif()

install(EXPORT ScopeGuard-config DESTINATION share/scopeguard/cmake)
