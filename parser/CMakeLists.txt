project(parser CXX)
cmake_minimum_required(VERSION 3.5)
include_directories(${CMAKE_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g3")
set(parser_src
  Grammar.cc
  Item.cc
  ParserBuilder.cc
  ParserTable.cc
  )
add_library(parser_lib ${parser_src})
add_executable(parser Parser.cc)
target_link_libraries(parser parser_lib)
