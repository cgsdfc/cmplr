project (cmplr)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")
set(tokenizer_src 
  lexer.c
  dfa.c
  error.c
  list.c
  chcl.c
  keyword.c
  token_type.c
  token.c
  char_buffer.c
  operator.c
  tokenizer.c
  )
add_library(lexer_lib ${tokenizer_src})
add_executable(lexer driver.c)
target_link_libraries(lexer lexer_lib)
