/* tokenizer.h */
#ifndef TOKENIZER_H
#define TOKENIZER_H 1
#include "char_buffer.h"
#include "token.h"

#define MAX_TRANSFER_ENTRIES 10

#define CHAR_CLASS_LOWER_CASE "abcdefghijklmnopqrstuvwxyz"
#define CHAR_CLASS_UPPER_CASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHAR_CLASS_TEN_DIGITS "0123456789" 
#define CHAR_CLASS_IDENTIFIER_BEGIN "_" CHAR_CLASS_LOWER_CASE CHAR_CLASS_UPPER_CASE
#define CHAR_CLASS_IDENTIFIER_PART CHAR_CLASS_IDENTIFIER_BEGIN CHAR_CLASS_TEN_DIGITS
#define CHAR_CLASS_SPACES " \t\n\r\f\v"
#define CHAR_CLASS_PUNCTUATION "(){}[]:;,.?"
#define CHAR_CLASS_OPERATOR "~!%^&*-+<>=/|"
#define CHAR_CLASS_BACKSLASH "\\"

#define CHAR_CLASS_SEPARATOR \
  CHAR_CLASS_SPACES CHAR_CLASS_PUNCTUATION CHAR_CLASS_OPERATOR

#define CHAR_CLASS_ONE_CHAR \
  CHAR_CLASS_PUNCTUATION CHAR_CLASS_OPERATOR

typedef void (* tkz_action) (struct token *, int, char_buffer *);


typedef struct transfer_entry 
{
  enum tokenizer_state state;
  bool is_accepted;
  bool is_reversed;
  char *char_class;
  tkz_action tkz_act;

} transfer_entry;

typedef enum tkz_error
{
  E_UNEXPECTED_CHAR=1,
  E_PREMATURE_END,
  E_NO_MORE_PUT_CHAR,

} tkz_error ;

typedef struct tk_status
{
  bool is_eof_hit;
  bool is_last_accepted;

} tk_status;

#endif

