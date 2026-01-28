cmake_minimum_required(VERSION 3.20)

# Qt UI Module (Optional)
# This module is opt-in and does not affect core builds

option(BUILD_UI_QT "Build Qt UI module" OFF)

if(BUILD_UI_QT)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
    
    message(STATUS "Qt6 found - Qt UI module available")
    
    set(CMAKE_AUTOMOC ON)
    
    add_executable(nx-ui-qt
        src/main.cpp
        src/app/QtApp.cpp
        src/app/QtApp.h
        src/adapters/QtCliAdapter.cpp
        src/adapters/QtCliAdapter.h
        src/ui/OutputView.cpp
        src/ui/OutputView.h
    )
    
    target_link_libraries(nx-ui-qt
        Qt6::Core
        Qt6::Widgets
        nx-cli-lib
    )
    
    target_include_directories(nx-ui-qt PRIVATE src)
    
else()
    message(STATUS "Qt UI module disabled (BUILD_UI_QT=OFF)")
endif()