cmake_minimum_required(VERSION 3.20)
project(nx-cli)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create library for testing
add_library(nx-cli-lib
    src/CliApp.cpp
    src/CommandRegistry.cpp
    src/adapters/MonitorQueryAdapter.cpp
    src/dto/MonitorStatusDto.cpp
    src/serialize/MonitorStatusJsonSerializer.cpp
    src/serialize/MonitorStatusTextSerializer.cpp
    src/error/CliError.cpp
    src/batch_artifact_loader.cpp
    src/batch_introspection_command.cpp
)

target_include_directories(nx-cli-lib PUBLIC include)
target_include_directories(nx-cli-lib PRIVATE src)

add_executable(nx-cli
    src/main.cpp
)

target_link_libraries(nx-cli nx-cli-lib)
target_include_directories(nx-cli PRIVATE src)

# Add tests subdirectory
add_subdirectory(tests)