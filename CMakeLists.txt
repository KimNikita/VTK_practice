cmake_minimum_required(VERSION 2.8)

project(VTK-PRACTICE)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(Lab_1 src/visualization.cpp)
target_link_libraries(Lab_1 ${VTK_LIBRARIES})