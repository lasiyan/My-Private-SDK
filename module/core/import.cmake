cmake_minimum_required(VERSION 3.20)
set(tag "core")

include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

# create header directory
make_directory(${__include_path}/${tag})
setup(${CMAKE_CURRENT_LIST_DIR}/${tag}.hpp true)    # main src

# sub src
setup(${CMAKE_CURRENT_LIST_DIR}/src/define.hpp)
setup(${CMAKE_CURRENT_LIST_DIR}/src/function.hpp)

setup(${CMAKE_CURRENT_LIST_DIR}/src/time.hpp)
setup(${CMAKE_CURRENT_LIST_DIR}/src/time.cpp)

setup(${CMAKE_CURRENT_LIST_DIR}/src/logger.hpp)
setup(${CMAKE_CURRENT_LIST_DIR}/src/logger.cpp)