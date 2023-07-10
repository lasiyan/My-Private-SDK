cmake_minimum_required(VERSION 3.00)
project(Total-SDK VERSION 1.3.1)

# set sdk naming
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

# set compiler options
include(cmake/compiler.cmake)

# set libraries
set(rowen_sdk_include )
include(cmake/macro.cmake)


# set library (default)
include(module/core/import.cmake)

# set libraries (optional)
OPTION(ENABLE_UTILS     "Enable Rowen SDK Utility Module"      ON)

if (ENABLE_UTILS)
include(module/util/import.cmake)
endif(ENABLE_UTILS)

# build static libraries
add_library(rowen STATIC ${rowen_sdk_source})


# samples
add_subdirectory(sample/core)
add_subdirectory(sample/thread-pool)