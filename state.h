#ifndef STATE_H
#define STATE_H 1

typedef enum tokenizer_state
{
  TK_INIT=0,
  TK_IDENTIFIER_BEGIN,
  TK_IDENTIFIER_END,
  TK_ONE_CHAR_BEGIN,
  TK_ONE_CHAR_END,
  
  TK_NULL

} tokenizer_state;

enum token_type;


#endif

