# NX-VideoTrans Engine - Phase 1.A Skeleton
cmake_minimum_required(VERSION 3.20)

project(nx-engine-video 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "NX-VideoTrans Core Engine - Phase 1.A Skeleton"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine library
add_library(nx-engine-video
    src/VideoEngine.cpp
)

target_include_directories(nx-engine-video
    PUBLIC include
    PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
)

target_link_libraries(nx-engine-video
    PRIVATE nx-core
)

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_executable(test_video_determinism
        tests/VideoEngine_determinism_tests.cpp
    )
    
    add_executable(test_deterministic_api
        tests/deterministic_api_tests.cpp
    )
    
    add_executable(test_minimal_determinism
        tests/minimal_determinism_proof.cpp
    )
    
    add_executable(test_video_simple
        tests/VideoEngine_simple_test.cpp
    )
    
    add_executable(test_video_skeleton
        tests/VideoEngine_skeleton_tests.cpp
    )
    
    target_link_libraries(test_video_determinism
        PRIVATE nx-engine-video
    )
    
    target_link_libraries(test_deterministic_api
        PRIVATE nx-engine-video
    )
    
    target_link_libraries(test_minimal_determinism
        PRIVATE nx-engine-video
    )
    
    target_link_libraries(test_video_simple
        PRIVATE nx-engine-video
    )
    
    target_link_libraries(test_video_skeleton
        PRIVATE nx-engine-video
    )
    
    target_include_directories(test_video_determinism
        PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
    )
    
    target_include_directories(test_deterministic_api
        PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
    )
    
    target_include_directories(test_minimal_determinism
        PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
    )
    
    target_include_directories(test_video_simple
        PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
    )
    
    target_include_directories(test_video_skeleton
        PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
    )
    
    enable_testing()
    add_test(NAME video_determinism_test COMMAND test_video_determinism)
    add_test(NAME deterministic_api_test COMMAND test_deterministic_api)
    add_test(NAME minimal_determinism_test COMMAND test_minimal_determinism)
    add_test(NAME video_simple_test COMMAND test_video_simple)
    add_test(NAME video_skeleton_test COMMAND test_video_skeleton)
endif()