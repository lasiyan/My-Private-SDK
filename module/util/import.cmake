cmake_minimum_required(VERSION 3.20)
set(tag "util")

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# create header directory
make_directory(${__include_path}/${tag})

# src
setup(${CMAKE_CURRENT_LIST_DIR}/src/threadPool.hpp)
setup(${CMAKE_CURRENT_LIST_DIR}/src/threadPool.cpp)
