file(GLOB sources main.cpp)
add_executable(sample_threadpool ${sources})
target_include_directories(sample_threadpool PUBLIC ${__install_path}/install)
target_link_libraries(sample_threadpool rowen)

if (NOT MSVC)
  target_link_libraries(sample_threadpool pthread)
endif()