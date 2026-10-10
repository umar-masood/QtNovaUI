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
    src/TextField.cpp
    src/ScrollBar.cpp
    src/Seperator.cpp

)

set(HEADERS 
    include/Button.h
    include/CheckBox.h 
    include/SpinnerProgress.h 
    include/Switch.h
    include/TextField.h
    include/ScrollBar.h
    include/Seperator.h
)

set(RESOURCES 
    resources/Icons.h
    resources/Icons.cpp

    resources/resources.qrc
)

add_executable(QtNovaUI
    # UI Components
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    
    main.cpp
)

target_link_libraries(QtNovaUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg)

set_target_properties(QtNovaUI PROPERTIES WIN32_EXECUTABLE TRUE)
