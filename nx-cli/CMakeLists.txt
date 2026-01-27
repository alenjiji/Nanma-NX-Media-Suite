cmake_minimum_required(VERSION 3.20)
project(nx-cli)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(nx-cli
    src/main.cpp
    src/CliApp.cpp
    src/CommandRegistry.cpp
)

target_include_directories(nx-cli PRIVATE src)