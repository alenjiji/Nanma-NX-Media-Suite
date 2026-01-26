cmake_minimum_required(VERSION 3.20)

# Test executable
add_executable(test_batch_engine test_batch_engine.cpp)
target_link_libraries(test_batch_engine nx-engine-batch)

# Add test to CTest
enable_testing()
add_test(NAME batch_engine_tests COMMAND test_batch_engine)