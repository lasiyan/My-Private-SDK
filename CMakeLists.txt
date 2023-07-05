cmake_minimum_required(VERSION 3.00)
project(Total-SDK VERSION 1.0)

add_compile_options(-Wno-format-truncation)

set(SDK_NAME rowen)
# set install configure
set(__install_path ${CMAKE_CURRENT_LIST_DIR}/install/${SDK_NAME}; )
set(__include_path ${__install_path}/include/${SDK_NAME}; )
set(__library_path ${__install_path}/lib; )
set(__execute_path ${__install_path}/bin; )
make_directory(${__install_path})
make_directory(${__include_path})
make_directory(${__library_path})
make_directory(${__execute_path})

set(CMAKE_INSTALL_PREFIX ${__install_path})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${__library_path})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${__library_path})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${__execute_path})

include_directories(${__include_path})
include_directories(${__include_path}/..)


# set libraries
set(rowen_sdk_include )
include(cmake/macro.cmake)


# set libraries (default)
include(module/core/import.cmake)


# build static libraries
add_library(rowen STATIC ${rowen_sdk_source})
