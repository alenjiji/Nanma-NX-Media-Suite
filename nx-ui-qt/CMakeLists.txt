cmake_minimum_required(VERSION 3.20)

# Qt UI Module (Optional)
# This module is opt-in and does not affect core builds

option(BUILD_UI_QT "Build Qt UI module" OFF)

if(BUILD_UI_QT)
    find_package(Qt6 QUIET COMPONENTS Core Widgets)
    
    if(Qt6_FOUND)
        message(STATUS "Qt6 found - Qt UI module available")
        
        set(CMAKE_AUTOMOC ON)
        
        add_executable(nx-ui-qt
            src/main.cpp
            src/app/QtApp.cpp
            src/app/QtApp.h
        )
        
        target_link_libraries(nx-ui-qt
            Qt6::Core
            Qt6::Widgets
        )
        
        target_include_directories(nx-ui-qt PRIVATE src)
        
    else()
        message(WARNING "Qt6 not found - Qt UI module disabled")
    endif()
else()
    message(STATUS "Qt UI module disabled (BUILD_UI_QT=OFF)")
endif()