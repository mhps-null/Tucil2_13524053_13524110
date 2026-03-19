cmake_minimum_required(VERSION 3.10)

project(Tucil2_13524053_13524110)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(program
src/main.cpp
src/geometry/Octree.cpp
src/geometry/Mesh.cpp
src/filesystem/FileSystem.cpp
)

target_include_directories(program PRIVATE src/header)