cmake_minimum_required(VERSION 3.16)
project(LeafMorph VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(LeafMorph
    src/main.cpp
    src/MainWindow.h
    src/MainWindow.cpp

    src/widgets/ToggleSwitch.h
    src/widgets/ToggleSwitch.cpp
    src/widgets/DropZone.h
    src/widgets/DropZone.cpp
    src/widgets/GlassCard.h
    src/widgets/GlassCard.cpp
    src/widgets/LeafIcon.h
    src/widgets/LeafIcon.cpp
    src/widgets/CustomCombo.h
    src/widgets/CustomCombo.cpp
    src/services/Converter.h
    src/services/Converter.cpp
    resources.qrc
)

target_link_libraries(LeafMorph PRIVATE Qt6::Widgets)

target_include_directories(LeafMorph PRIVATE
    src
    src/widgets
)

set_target_properties(LeafMorph PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)