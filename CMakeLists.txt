cmake_minimum_required(VERSION 3.5)
add_subdirectory(src/lexer)
add_subdirectory(src/parser)
include_directories(tknzr parser)

