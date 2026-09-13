cmake_minimum_required(VERSION 3.16)
project(QtNovaUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Svg)

set(SOURCES 
    src/Button.cpp
    src/CheckBox.cpp 
    src/SpinnerProgress.cpp 
    src/Switch.cpp
)

set(HEADERS 
    include/Button.h
    include/CheckBox.h 
    include/SpinnerProgress.h 
    include/Switch.h
)

add_executable(QtNovaUI
    # UI Components
    ${SOURCES}
    ${HEADERS}
    
    main.cpp
)

target_link_libraries(QtNovaUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg)

set_target_properties(QtNovaUI PROPERTIES WIN32_EXECUTABLE TRUE)
