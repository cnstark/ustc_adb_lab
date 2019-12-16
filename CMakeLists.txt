cmake_minimum_required(VERSION 3.15)
project(adb_project)

set(CMAKE_CXX_STANDARD 11)

add_executable(adb_project src/main.cpp src/data_storage_manager.cpp src/buffer_manager.cpp src/common.cpp)
