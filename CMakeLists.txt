cmake_minimum_required(VERSION 3.23)
project(graph-view)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 CONFIG REQUIRED PATHS C:/Libraries/SDL3-3.4.14)

add_library(glad STATIC glad/src/glad.c)

target_include_directories(glad PUBLIC glad/include)

add_executable(App)

target_sources(App PRIVATE main_gl.c src/utils.c src/window_app.c src/node.c)

target_link_libraries(App PRIVATE SDL3::SDL3 glad)
