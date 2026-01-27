cmake_minimum_required(VERSION 3.20)

# Test executables
set(TEST_SOURCES
    test_batch_engine.cpp
    test_batch_plan_session.cpp
    test_execution_interfaces.cpp
    test_execution_state.cpp
    test_deterministic_execution_engine.cpp
    test_execution_persistence.cpp
    test_retry_engine.cpp
    test_replay_driver.cpp
)

# Create test executables
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    
    target_link_libraries(${TEST_NAME} PRIVATE nx-engine-batch nx-core)
    
    # Compiler warnings
    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
    
    # Add to test suite
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Enable testing
enable_testing()