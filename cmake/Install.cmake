
install(TARGETS ScopeGuard EXPORT ScopeGuardConfig DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if( SCOPEGUARD_ENABLE_COMPAT_HEADER )
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generated/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

install(EXPORT ScopeGuardConfig
    NAMESPACE ScopeGuard::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/ScopeGuard/cmake
    )
