file(GLOB sources main.cpp)
add_executable(sample_core ${sources})
target_link_libraries(sample_core rowen)