cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS "-g3")
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(experiment)
include_directories(lexer parser)

