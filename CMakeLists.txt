cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS "-g3")
add_subdirectory(src/lexer)
add_subdirectory(src/parser)
add_subdirectory(src/experiment)
include_directories(tknzr parser)

