cmake_minimum_required(VERSION 3.5)
include_directories(${CMAKE_SOURCE_DIR})
add_definitions(-DDEBUG_DIGRAPH)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(parser_src
  lookahead_set.c
  bitset.c
  digraph.c
  grammar.c
  rules.c
  simple.c
  item.c
  )
add_library(parser_lib ${parser_src})
add_executable(parser parser.c)
target_link_libraries(parser parser_lib)
