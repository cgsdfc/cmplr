#ifndef STATE_H
#define STATE_H 1
#define N_NON_ACCEPTED_STATES    (_TK_NON_ACCEPTED_END-TK_INIT)
#define MAX_TRANSFER_ENTRIES N_NON_ACCEPTED_STATES


extern const char *token_state_tab[];

typedef enum tokenizer_state
{
  /* NOT ACCEPTED STATES */
  TK_INIT=0,
  TK_IDENTIFIER_BEGIN,
  TK_PUNCTUATION_BEGIN,
  TK_INT_DEC_BEGIN,
  TK_INT_HEX_OCT_BEGIN,
  TK_INT_HEX_BEGIN,
  TK_INT_OCT_BEGIN,
  TK_INT_LONG,
  TK_INT_UNSIGNED,

  TK_SINGLE_LINE_COMENT_BEGIN,
  TK_MULTI_LINE_COMENT_BEGIN,
  TK_CHAR_LITERAL_BEGIN,
  TK_CHAR_LITERAL_HEX_BEGIN,
  TK_CHAR_LITERAL_OCT_BEGIN,
  TK_STRING_LITERAL_BEGIN,
  TK_CHAR_LITERAL_ZERO,
  TK_CHAR_LITERAL_ESCAPED,
  TK_CHAR_LITERAL_PART,
  TK_CHAR_LITERAL_OCT_END,
  TK_CHAR_LITERAL_HEX_END,
  TK_SINGLE_LINE_COMENT_END,
  TK_MULTI_LINE_COMENT_END,
  
  /* NON ACCEPTED OPERATOR STATES */ 
  TK_EXCLAIM,
  TK_EQUAL,
  TK_STAR,
  TK_PERCENT,
  TK_CARET,
  TK_SLASH,

  /* 4 states */
  TK_AMPERSAND,
  TK_VERTICAL_BAR,
  TK_POSITIVE,
  TK_NEGATIVE,

  /* 6 states */
  TK_LESS,
  TK_GREATER,
  TK_GREATER_GREATER,
  TK_LESS_LESS,
  /*!NON ACCEPTED OPERATOR STATES */ 
  _TK_NON_ACCEPTED_END,
  /*!NOT ACCEPTED STATES */

  /* ACCEPTED STATES */
  TK_IDENTIFIER_END,
  TK_INT_END,
  TK_PUNCTUATION_END,
  TK_CHAR_LITERAL_END,
  TK_STRING_LITERAL_END,


  /* ACCEPTED OPERATOR STATES */
  _TK_OPERATOR_ACCEPT_BEGIN,
  /* PUT BACH OPERATOR STATES */
   /* 3 states */
  TK_EXCLAIM_END,
  TK_EQUAL_END,
  TK_STAR_END,
  TK_PERCENT_END,
  TK_CARET_END,
  TK_SLASH_END,

  /* 4 states */
  TK_AMPERSAND_END,
  TK_VERTICAL_BAR_END,
  TK_POSITIVE_END,
  TK_NEGATIVE_END,

  /* 6 states */
  TK_GREATER_END,
  TK_GREATER_GREATER_END,
  TK_LESS_END,
  TK_LESS_LESS_END,
  _TK_OPERATOR_PUT_BACK,
  /*!PUT BACH OPERATOR STATES */

  TK_TILDE,
  TK_AMPERSAND_AMPERSAND,
  TK_LESS_LESS_EQUAL,
  TK_GREATER_GREATER_EQUAL,
  TK_VERTICAL_BAR_BAR,
  TK_POSITIVE_POSITIVE,
  TK_NEGATIVE_NEGATIVE,
  TK_NEGATIVE_GREATER,

  TK_EXCLAIM_EQUAL,
  TK_PERCENT_EQUAL,
  TK_CARET_EQUAL,
  TK_AMPERSAND_EQUAL,
  TK_STAR_EQUAL,
  TK_POSITIVE_EQUAL,
  TK_NEGATIVE_EQUAL,
  TK_LESS_EQUAL,
  TK_GREATER_EQUAL,
  TK_EQUAL_EQUAL,
  TK_SLASH_EQUAL,
  TK_VERTICAL_BAR_EQUAL,
  _TK_OPERATOR_ACCEPT_END,
  /*!ACCEPTED OPERATOR STATES */
  /*!OPERATOR STATES */
  /*!ACCEPTED STATES */

  
/* SPECIAL STATE FOR FAILURE */
  TK_NULL 
} tokenizer_state;

enum token_type;
typedef tokenizer_state node;


#endif

