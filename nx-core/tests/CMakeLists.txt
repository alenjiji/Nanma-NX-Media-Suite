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

# API contract tests
add_executable(test_api_contract test_api_contract.cpp)
target_link_libraries(test_api_contract nx-core)

# BatchFlow workflow tests
add_executable(test_batchflow_workflow test_batchflow_workflow.cpp)
target_link_libraries(test_batchflow_workflow nx-core)

# Critical determinism tests
add_executable(test_determinism_critical test_determinism_critical.cpp)
target_link_libraries(test_determinism_critical nx-core)

# Law violation tests
add_executable(test_law_violations test_law_violations.cpp)
target_link_libraries(test_law_violations nx-core)

# Replay proof tests (Crown Jewel)
add_executable(test_replay_proof test_replay_proof.cpp)
target_link_libraries(test_replay_proof nx-core)

# Register tests with CTest
add_test(NAME identity_tests COMMAND test_nx_identity)
add_test(NAME logical_clock_tests COMMAND test_logical_clock)
add_test(NAME error_system_tests COMMAND test_error_system)
add_test(NAME result_tests COMMAND test_result)
add_test(NAME api_contract_tests COMMAND test_api_contract)
add_test(NAME batchflow_workflow_tests COMMAND test_batchflow_workflow)
add_test(NAME determinism_critical_tests COMMAND test_determinism_critical)
add_test(NAME law_violation_tests COMMAND test_law_violations)
add_test(NAME replay_proof_tests COMMAND test_replay_proof)