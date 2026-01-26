cmake_minimum_required(VERSION 3.20)

# Test executable
add_executable(test_cli test_cli.cpp)
target_link_libraries(test_cli nx-cli-lib)

# MetaFix test executable
add_executable(test_metafix_cli test_metafix_cli.cpp)
target_link_libraries(test_metafix_cli nx-cli-lib)

# Add test to CTest
enable_testing()
add_test(NAME cli_tests COMMAND test_cli)
add_test(NAME metafix_cli_tests COMMAND test_metafix_cli)