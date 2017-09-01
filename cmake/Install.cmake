
install(TARGETS ScopeGuard EXPORT ScopeGuard-config DESTINATION include)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" DESTINATION include)
install(EXPORT ScopeGuard-config DESTINATION share/scopeguard/cmake)
