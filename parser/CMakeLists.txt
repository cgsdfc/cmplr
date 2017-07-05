cmake_minimum_required(VERSION 3.5)
include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(parser_src
  rules.c
  )
add_library(parser_lib ${parser_src})
add_executable(parser parser.c)
target_link_libraries(parser parser_lib)
