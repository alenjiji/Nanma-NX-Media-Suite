cmake_minimum_required(VERSION 3.20)

# Test executable
add_executable(test_cli test_cli.cpp)
target_link_libraries(test_cli nx-cli-lib)

# MetaFix test executable
add_executable(test_metafix_cli test_metafix_cli.cpp)
target_link_libraries(test_metafix_cli nx-cli-lib)

# Audio test executable
add_executable(test_audio_cli test_audio_cli.cpp)
target_link_libraries(test_audio_cli nx-cli-lib)

# Video test executable
add_executable(test_video_cli test_video_cli.cpp)
target_link_libraries(test_video_cli nx-cli-lib)

# Batch test executable
add_executable(test_batch_cli test_batch_cli.cpp)
target_link_libraries(test_batch_cli nx-cli-lib)

# Monitor test executable
add_executable(test_monitor_cli test_monitor_cli.cpp)
target_link_libraries(test_monitor_cli nx-cli-lib)

# Batch introspection golden test executable
add_executable(test_batch_introspection_golden test_batch_introspection_golden.cpp)
target_link_libraries(test_batch_introspection_golden nx-cli-lib)

# Batch status and job commands test executable
add_executable(test_batch_status_job_commands test_batch_status_job_commands.cpp)
target_link_libraries(test_batch_status_job_commands nx-cli-lib)

# Batch policies command test executable
add_executable(test_batch_policies_command test_batch_policies_command.cpp)
target_link_libraries(test_batch_policies_command nx-cli-lib)

# Batch artifacts commands test executable
add_executable(test_batch_artifacts_commands test_batch_artifacts_commands.cpp)
target_link_libraries(test_batch_artifacts_commands nx-cli-lib)

# Add test to CTest
enable_testing()
add_test(NAME cli_tests COMMAND test_cli)
add_test(NAME metafix_cli_tests COMMAND test_metafix_cli)
add_test(NAME audio_cli_tests COMMAND test_audio_cli)
add_test(NAME video_cli_tests COMMAND test_video_cli)
add_test(NAME batch_cli_tests COMMAND test_batch_cli)
add_test(NAME monitor_cli_tests COMMAND test_monitor_cli)
add_test(NAME batch_introspection_golden_tests COMMAND test_batch_introspection_golden)
add_test(NAME batch_status_job_tests COMMAND test_batch_status_job_commands)
add_test(NAME batch_policies_tests COMMAND test_batch_policies_command)
add_test(NAME batch_artifacts_tests COMMAND test_batch_artifacts_commands)