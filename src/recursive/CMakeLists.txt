project(recursive C)
include_directories(${CMAKE_SOURCE_DIR})



add_library(recursive
  ./repeated.c
  ./expr.c
)
target_link_libraries(recursive
  lexer_lib
)
