cmake_minimum_required(VERSION 3.16.0)

project(MPCCT VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# VTK
find_package(VTK REQUIRED)

add_executable(${PROJECT_NAME}
					src/resources.qrc
    				src/main.cpp
					src/MainWindow.cpp
					src/MainWindow.h
					src/Draw.cpp
					src/Draw.h
					src/Camera.cpp
					src/Camera.h
					src/Project.cpp
					src/Project.h
					src/CaptureScreenshotsWidget.cpp
					src/CaptureScreenshotsWidget.h
					src/json.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets)

target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})

