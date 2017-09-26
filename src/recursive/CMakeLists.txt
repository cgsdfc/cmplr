project(recursive C)
include_directories(${CMAKE_SOURCE_DIR})



add_library(recursive
  ./stmt.c
  ./error.c
  ./terminal.c
  ./expr.c
  ./node_base.c
)
target_link_libraries(recursive
  lexer_lib
)

add_executable(rp ./driver.c)
target_link_libraries(rp
  recursive)
