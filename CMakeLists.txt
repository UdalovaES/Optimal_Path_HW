cmake_minimum_required(VERSION 3.2)
project(main)
set(CMAKE_CXX_STANDARD 17)
add_executable(
${PROJECT_NAME} src/main.cpp
        "src/Vertice_Direction.h"
)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/load_png)
add_subdirectory(load_png)
target_link_libraries(${PROJECT_NAME} lodepng)