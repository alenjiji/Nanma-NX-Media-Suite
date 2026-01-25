cmake_minimum_required(VERSION 3.20)
project(nanma-nx-media-suite VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(nx-core)
add_subdirectory(nx-engine-convert)
add_subdirectory(nx-engine-audio)
add_subdirectory(nx-engine-video)
add_subdirectory(nx-engine-meta)