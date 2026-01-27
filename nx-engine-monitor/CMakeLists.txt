cmake_minimum_required(VERSION 3.20)

project(nx-engine-monitor VERSION 1.0.0 LANGUAGES CXX)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include)

# Source files
set(MONITOR_ENGINE_SOURCES
    src/NullMonitorEngine.cpp
    src/RealMonitorEngine.cpp
    src/ExecutionBoundaryObserver.cpp
)

# Monitor Engine Library
add_library(nx-engine-monitor ${MONITOR_ENGINE_SOURCES})
target_include_directories(nx-engine-monitor PUBLIC include)

# Dependencies
target_include_directories(nx-engine-monitor PRIVATE 
    ${CMAKE_SOURCE_DIR}/nx-engine-batch/include
)

# Compiler warnings
if(MSVC)
    target_compile_options(nx-engine-monitor PRIVATE /W4)
else()
    target_compile_options(nx-engine-monitor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests (optional)
option(BUILD_MONITOR_TESTS "Build monitor engine tests" OFF)
if(BUILD_MONITOR_TESTS)
    add_subdirectory(tests)
endif()