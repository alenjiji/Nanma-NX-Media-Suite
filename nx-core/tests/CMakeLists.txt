# NOTE:
# NX-Core must never link against:
# - Qt
# - Engine modules
# - UI libraries
# Violations invalidate architectural guarantees.

cmake_minimum_required(VERSION 3.20)

# Basic test infrastructure for NX-Core
# Tests will validate foundational primitives only

# Identity tests
add_executable(test_nx_identity test_nx_identity.cpp)
target_link_libraries(test_nx_identity nx-core)

# Logical clock tests
add_executable(test_logical_clock test_logical_clock.cpp)
target_link_libraries(test_logical_clock nx-core)

# Error system tests
add_executable(test_error_system test_error_system.cpp)
target_link_libraries(test_error_system nx-core)

# Result type tests
add_executable(test_result test_result.cpp)
target_link_libraries(test_result nx-core)

# Register tests with CTest
add_test(NAME identity_tests COMMAND test_nx_identity)
add_test(NAME logical_clock_tests COMMAND test_logical_clock)
add_test(NAME error_system_tests COMMAND test_error_system)
add_test(NAME result_tests COMMAND test_result)