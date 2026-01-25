# NX-Convert Pro Engine - Phase 1.A Skeleton
cmake_minimum_required(VERSION 3.20)

project(nx-engine-convert 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "NX-Convert Pro Core Engine - Phase 1.A Skeleton"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine library
add_library(nx-engine-convert
    src/TranscodeEngine.cpp
)

target_include_directories(nx-engine-convert
    PUBLIC include
)

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_executable(minimal_determinism
        tests/minimal_determinism.cpp
    )
    
    target_link_libraries(minimal_determinism
        PRIVATE nx-engine-convert
    )
    
    enable_testing()
    add_test(NAME minimal_determinism_test COMMAND minimal_determinism)
endif()