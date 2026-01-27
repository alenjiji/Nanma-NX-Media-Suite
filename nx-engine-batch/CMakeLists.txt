cmake_minimum_required(VERSION 3.20)

project(nx-engine-batch VERSION 1.0.0 LANGUAGES CXX)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include)

# Source files
set(BATCH_ENGINE_SOURCES
    src/BatchEngineImpl.cpp
    src/BatchPlanSession.cpp
    src/ExecutionInput.cpp
    src/ExecutionGraph.cpp
    src/ExecutionCoordinator.cpp
)

# Batch Engine Library
add_library(nx-engine-batch ${BATCH_ENGINE_SOURCES})
target_include_directories(nx-engine-batch PUBLIC include)

# Dependencies
target_include_directories(nx-engine-batch PRIVATE 
    ${CMAKE_SOURCE_DIR}/nx-core/include
)

# Compiler warnings
if(MSVC)
    target_compile_options(nx-engine-batch PRIVATE /W4)
else()
    target_compile_options(nx-engine-batch PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Tests (optional)
option(BUILD_BATCH_TESTS "Build batch engine tests" OFF)
if(BUILD_BATCH_TESTS)
    add_subdirectory(tests)
endif()