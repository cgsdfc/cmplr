project (cmplr)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(tokenizer_src 
  tknzr_tab.c
  parser_error.c
  tknzr_error.c
  comment.c
  identifier.c
  escaped.c
  checker.c
  float_literal.c
  integer_literal.c
  tknzr_table.c  
  state_table.c
  char_class.c
  string_literal.c
  char_literal.c
  keyword.c
  token_type.c
  punctuation.c
  token.c
  tknzr_state.c
  char_buffer.c
  operator.c
  )
add_library(tokenizer_lib ${tokenizer_src})
add_executable(tokenizer tokenizer.c)
target_link_libraries(tokenizer tokenizer_lib)


add_executable(intlit intlit.c)
add_executable(t t.c)
