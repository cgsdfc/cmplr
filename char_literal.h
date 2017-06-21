#ifndef CHAR_LITERAL_H 
#define CHAR_LITERAL_H 1

#include "tokenizer.h"
#include "state.h"


#define CHAR_CLASS_CHAR_LITERAL " `\"#@$" CHAR_CLASS_IDENTIFIER_PART\
  CHAR_CLASS_OPERATOR CHAR_CLASS_PUNCTUATION

#define CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO "abfnrtv\\\'\"?"
#define CHAR_CLASS_SINGLE_ESCAPE "0" CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO 
#define CHAR_CLASS_HEX_LETTER "abcdefABCDEF"
#define CHAR_CLASS_HEX_BEGIN CHAR_CLASS_DEC_PART CHAR_CLASS_HEX_LETTER
#define CHAR_CLASS_OCT_BEGIN_NOT_ZERO "1234567"
#define CHAR_CLASS_OCT_BEGIN "0" CHAR_CLASS_OCT_BEGIN_NOT_ZERO
#define CHAR_CLASS_SINGLE_QUOTE "\'"
#define CHAR_CLASS_DOUBLE_QUOTE "\""

void init_char_literal (void);
void add_initial(node from, char *char_class);
void add_intermedia_rev (node from, node to, char *char_class ); 
void add_intermedia (node from, node to, char* char_class); 
void add_selfloop (node from, char *char_class);;
void add_selfloop_rev (node from, char *char_class);;
void add_accepted_rev(node from,  node to, char * char_class); 
void add_accepted(node from, node to , char *char_class); 
#endif

