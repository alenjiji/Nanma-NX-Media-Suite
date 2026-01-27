# NOTE:
# NX-Core must never link against:
# - Qt
# - Engine modules
# - UI libraries
# Violations invalidate architectural guarantees.

cmake_minimum_required(VERSION 3.20)
project(nx-core VERSION 1.0.0 LANGUAGES CXX)

# C++20 requirement
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# NX-Core library target
set(NX_CORE_SOURCES
    src/nx_core.cpp
    src/identity.cpp
    src/result.cpp
    src/api_contract.cpp
    src/determinism_guards.cpp
    src/deterministic_numeric_policy.cpp
)

add_library(nx-core ${NX_CORE_SOURCES})
target_include_directories(nx-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Architectural constraint enforcement
# NX-Core must have NO dependencies on higher layers
# NX-Core must be Qt-free
# NX-Core must be media-agnostic

# Optional: Build as shared library
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()