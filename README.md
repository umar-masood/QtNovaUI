<div align="center">

# QtNovaUI

**Modern and Customizable UI components for Qt (C++)**
Designed for Windows & macOS · Built with Qt 6 (C++)

![Qt](https://img.shields.io/badge/Qt-6-41CD52?logo=qt&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C?logo=cmake&logoColor=white)
![Platforms](https://img.shields.io/badge/Platforms-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey)

</div>

---

## Overview

QtNovaUI is a collection of hand-built, custom Qt Widgets components that give desktop applications a modern, smooth and responsive look. Every component is written in C++ with Qt and custom painting, so it stays lightweight and easy to tweak to match your app's design.

## Features

- **Modern look and feel**: rounded corners and smooth transitions
- **Highly customizable**: colors, radii, icons and styles can be adjusted per component
- **Native C++ / Qt Widgets**: no extra runtime and no QML required
- **Cross-platform**: designed for Windows, Linux and MacOS
- **CMake based**: easy to drop into an existing project

## Components
| Category | Components |
| --- | --- |
| **Inputs** | Button, CheckBox, TextField, Toggle |
| **Feedback** | SpinnerProgress |
| **Navigation** | ScrollBar |

> [!NOTE]
> More components are under development.

## Requirements
- **Qt 6** with the following modules: `Core`, `Gui`, `Widgets`, `Svg`
- **CMake** 3.16 or newer
- A C++17 compatible compiler (MSVC, MinGW, Clang or GCC)
- Windows, MacOS or Linux

## Getting Started
### 1. Clone the repository

```bash
git clone https://github.com/umar-masood/QtNovaUI.git
```

### 2. Add the components to your project
Copy the components you need (the `.h` / `.cpp` pairs, plus the `resources` folder for icons) into your project, or keep QtNovaUI as a subfolder / git submodule:

```bash
git submodule add https://github.com/umar-masood/QtNovaUI.git external/QtNovaUI
```

### 3. Update your `CMakeLists.txt`
```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Svg)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_executable(MyApp
    main.cpp

    # QtNovaUI components (add only what you use)
    external/QtNovaUI/src/Button.cpp
    external/QtNovaUI/include/Button.h
    external/QtNovaUI/src/CheckBox.cpp
    external/QtNovaUI/include/CheckBox.h
    external/QtNovaUI/src/TextField.cpp
    external/QtNovaUI/include/TextField.h

    # Icons / resources
    external/QtNovaUI/resources/Icons.h
    external/QtNovaUI/resources/Icons.cpp
    external/QtNovaUI/resources/resources.qrc
)

target_include_directories(MyApp PRIVATE external/QtNovaUI)
target_link_libraries(MyApp PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Svg)
```

## Project Structure
```text
QtNovaUI/
├── include/            # Public headers (.h)
│   ├── Button.h
│   ├── CheckBox.h
│   ├── ScrollBar.h
│   ├── Seperator.h
│   ├── SpinnerProgress.h
│   ├── TextField.h
│   ├── Toggle.h
├── src/                # Implementations (.cpp)
│   ├── Button.cpp
│   ├── CheckBox.cpp
│   ├── ScrollBar.cpp
│   ├── Seperator.cpp
│   ├── SpinnerProgress.cpp
│   ├── TextField.cpp
│   ├── Toggle.cpp
├── resources/          # Icons and Qt resource files
└── CMakeLists.txt
```
## Author
**Umar Masood** - [@umar-masood](https://github.com/umar-masood)

If you find QtNovaUI useful, consider giving it a ⭐ on GitHub!