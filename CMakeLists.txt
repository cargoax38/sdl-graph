cmake_minimum_required(VERSION 3.23)
project(graph-view)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 CONFIG REQUIRED PATHS C:/Libraries/SDL3-3.4.14)

add_executable(App)

target_sources(App PRIVATE main.c src/mat4d.c src/vec4d.c src/camera.c)

target_link_libraries(App PRIVATE SDL3::SDL3)
