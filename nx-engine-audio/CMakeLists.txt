# NX-AudioLab Engine - Phase 1.A Skeleton
cmake_minimum_required(VERSION 3.20)

project(nx-engine-audio 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "NX-AudioLab Core Engine - Phase 1.A Skeleton"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine library
add_library(nx-engine-audio
    src/AudioEngine.cpp
)

target_include_directories(nx-engine-audio
    PUBLIC include
    PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
)

target_link_libraries(nx-engine-audio
    PRIVATE nx-core
)

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_executable(test_audio_determinism
        tests/AudioEngine_determinism_tests.cpp
    )
    
    add_executable(deterministic_api_tests
        tests/deterministic_api_tests.cpp
    )
    
    target_link_libraries(test_audio_determinism
        PRIVATE nx-engine-audio
    )
    
    target_link_libraries(deterministic_api_tests
        PRIVATE nx-engine-audio
    )
    
    enable_testing()
    add_test(NAME audio_determinism_test COMMAND test_audio_determinism)
    add_test(NAME deterministic_api_test COMMAND deterministic_api_tests)
endif()