cmake_minimum_required(VERSION 3.10)

project(Tucil2_13524053_13524110)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(voxelizer
    src/main.cpp
    src/geometry/Octree.cpp
    src/geometry/Mesh.cpp
    src/filesystem/FileSystem.cpp
)

add_executable(viewer
    src/objectviewer/CameraLogic.cpp
    src/objectviewer/Viewer.cpp
    src/geometry/Mesh.cpp
    src/filesystem/FileSystem.cpp
)

target_include_directories(voxelizer PRIVATE src/header)
target_include_directories(viewer PRIVATE src/header)

target_link_libraries(viewer PRIVATE sfml-graphics sfml-window sfml-system)