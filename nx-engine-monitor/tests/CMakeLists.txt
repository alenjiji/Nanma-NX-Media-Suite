cmake_minimum_required(VERSION 3.20)

# Test executable
add_executable(test_null_monitor_engine test_null_monitor_engine.cpp)
target_link_libraries(test_null_monitor_engine nx-engine-monitor)

# Add test to CTest
enable_testing()
add_test(NAME null_monitor_engine_tests COMMAND test_null_monitor_engine)