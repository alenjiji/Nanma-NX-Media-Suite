cmake_minimum_required(VERSION 3.20)

project(nx-cli VERSION 1.0.0 LANGUAGES CXX)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include)

# Source files
set(CLI_SOURCES
    src/argument_parser.cpp
    src/convert_command.cpp
    src/metafix_argument_parser.cpp
    src/metafix_command.cpp
    src/audio_argument_parser.cpp
    src/audio_command.cpp
    src/video_argument_parser.cpp
    src/video_command.cpp
    src/batch_argument_parser.cpp
    src/batch_command.cpp
    src/nx_cli.cpp
)

# CLI Library
add_library(nx-cli-lib ${CLI_SOURCES})
target_include_directories(nx-cli-lib PUBLIC include)

# Main executable
add_executable(nx src/main.cpp)
target_link_libraries(nx nx-cli-lib)

# Compiler warnings
if(MSVC)
    target_compile_options(nx-cli-lib PRIVATE /W4)
    target_compile_options(nx PRIVATE /W4)
else()
    target_compile_options(nx-cli-lib PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(nx PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests (optional)
option(BUILD_CLI_TESTS "Build CLI tests" OFF)
if(BUILD_CLI_TESTS)
    add_subdirectory(tests)
endif()