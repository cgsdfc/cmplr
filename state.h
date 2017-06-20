#ifndef STATE_H
#define STATE_H 1

extern const char *token_state_tab[];

typedef enum tokenizer_state
{
  TK_INIT=0,
  TK_IDENTIFIER_BEGIN,
  TK_IDENTIFIER_END,
  TK_ONE_CHAR_BEGIN,
  TK_ONE_CHAR_END,
  TK_INT_DEC_BEGIN,
  TK_INT_HEX_OCT_BEGIN,
  TK_INT_HEX_BEGIN,
  TK_INT_OCT_BEGIN,
  TK_INT_END,
  TK_INT_LONG,
  TK_INT_UNSIGNED,
  
  TK_NULL

} tokenizer_state;

enum token_type;


#endif

