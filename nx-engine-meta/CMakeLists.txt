# NX-MetaFix Engine - Phase 1.A Skeleton
cmake_minimum_required(VERSION 3.20)

project(nx-engine-meta 
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "NX-MetaFix Core Engine - Phase 1.A Skeleton"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Engine library
add_library(nx-engine-meta
    src/MetaEngine.cpp
)

target_include_directories(nx-engine-meta
    PUBLIC include
    PRIVATE ${CMAKE_SOURCE_DIR}/nx-core/include
)

target_link_libraries(nx-engine-meta
    PRIVATE nx-core
)

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    add_executable(test_meta_determinism
        tests/MetaEngine_determinism_tests.cpp
    )
    
    target_link_libraries(test_meta_determinism
        PRIVATE nx-engine-meta
    )
    
    enable_testing()
    add_test(NAME meta_determinism_test COMMAND test_meta_determinism)
endif()