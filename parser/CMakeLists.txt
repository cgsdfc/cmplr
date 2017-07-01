cmake_minimum_required(VERSION 3.5)
include_directories(${CMAKE_SOURCE_DIR})
set(parser_src
  expression.c
  statement.c
  hash_table.c
  declaration.c
  abstract_syntax_tree.c
  )
add_library(parser_lib ${parser_src})
target_link_libraries(parser_lib tokenizer_lib)
add_executable(parser parser_driver.c)
target_link_libraries(parser parser_lib)
