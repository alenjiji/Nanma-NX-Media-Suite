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
    add_executable(test_video_skeleton
        tests/VideoEngine_skeleton_tests.cpp
    )
    
    target_link_libraries(test_video_skeleton
        PRIVATE nx-engine-video
    )
    
    enable_testing()
    add_test(NAME video_skeleton_test COMMAND test_video_skeleton)
endif()