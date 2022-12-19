
install(TARGETS ScopeGuard EXPORT ScopeGuardConfig DESTINATION include)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION include)

if( ENABLE_COMPAT_HEADER )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/" DESTINATION include)
endif()

install(EXPORT ScopeGuardConfig DESTINATION share/ScopeGuard/cmake)
