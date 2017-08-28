project (cmplr)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(tokenizer_src 
  dfa.c
  block_buffer.c
  token_buffer.c
  parser_error.c
  tknzr_error.c
  chcl.c
  keyword.c
 token_type.c
  token.c
  char_buffer.c
  operator.c
  tokenizer.c
  )
add_library(lexer_lib ${tokenizer_src})
add_executable(lexer tknzr_driver.c)
target_link_libraries(lexer lexer_lib)
