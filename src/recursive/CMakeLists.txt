project(recursive C)
add_executable(simple_expr simple_expr.c)
target_link_libraries(simple_expr lexer_lib)

