project(recursive C)
include_directories(${CMAKE_SOURCE_DIR})
add_library(recursive
  ./construct.c
  ./print_xml.c
  ./decl.c
  ./struct.c
  ./decltor.c
  ./specifier.c
  ./util.c
  ./context.c
  ./parser.c
  ./stmt.c
  ./error.c
  ./terminal.c
  ./expr.c
  ./node_base.c
)
target_link_libraries(recursive
  lexer_lib
  utillib
)

add_executable(rp ./driver.c)
target_link_libraries(rp
  recursive)
