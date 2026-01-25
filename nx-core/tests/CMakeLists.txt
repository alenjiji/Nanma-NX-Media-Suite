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

# Register test with CTest
add_test(NAME identity_tests COMMAND test_nx_identity)