#ifndef CHAR_CLASS_H
#define CHAR_CLASS_H 1
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "state_table.h"


#define CC_DEC_DIGITS "0123456789" 
#define CC_DEC_DIGITS_NON_ZERO "123456789" 
#define CC_HEX_LETTER "abcdefABCDEF"
#define CC_HEX_DIGITS CC_DEC_DIGITS CC_HEX_LETTER
#define CC_OCT_DIGITS_NOT_ZERO "1234567"
#define CC_OCT_DIGITS "0" CC_OCT_DIGITS_NOT_ZERO
#define CC_SINGLE_QUOTE "\'"
#define CC_DOUBLE_QUOTE "\""

#define CC_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CC_UPPER  "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CC_IDENTIFIER_BEGIN "_" CC_LOWER CC_UPPER
#define CC_IDENTIFIER_PART CC_IDENTIFIER_BEGIN CC_DEC_DIGITS
#define CC_SPACES " \t\n\r\f\v"
#define CC_NEWLINE "\n\r"
#define CC_PUNCTUATION_NON_DOT "(){}[]:;,?"
#define CC_OPERATOR "~!%^&*-+<>=/|"
#define CC_BACKSLASH "\\"
#define CC_E "eE"
#define CC_X "x"
#define CC_ZERO "0"
#define CC_DOT "."
#define CC_SIGN "-+"
#define CC_INT_SUFFIX "lLuU"
#define CC_FLOAT_SUFFIX "lLfF"
#define CC_SINGLE_ESCAPE "abfnrtv\\\'\"?"

typedef enum char_class
{
  /* legal char class id starts from 1 */
  CHAR_CLASS_EMPTY=1,

  /* id */
  CHAR_CLASS_IDENTIFIER_BEGIN,
  CHAR_CLASS_IDENTIFIER_PART,
  CHAR_CLASS_SPACES,
  CHAR_CLASS_NEWLINE,
  CHAR_CLASS_PUNCTUATION_NON_DOT,

  /* number */
  CHAR_CLASS_DOT,
  CHAR_CLASS_ZERO,
  CHAR_CLASS_DEC_DIGITS,
  CHAR_CLASS_LETTER,
  CHAR_CLASS_E,
  CHAR_CLASS_DEC_DIGITS_LETTERS,
  CHAR_CLASS_DEC_DIGITS_LETTERS_DOT,
  CHAR_CLASS_DEC_DIGITS_NON_ZERO,
  CHAR_CLASS_SIGN,
  CHAR_CLASS_XX,
  CHAR_CLASS_HEX_DIGITS,
  CHAR_CLASS_OCT_DIGITS,
  CHAR_CLASS_FLOAT_SUFFIX,
  CHAR_CLASS_INT_SUFFIX,
  CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX ,
  CHAR_CLASS_DEC_DIGITS_INT_SUFFIX ,
  CHAR_CLASS_OCT_DIGITS_INT_SUFFIX ,
  CHAR_CLASS_HEX_DIGITS_INT_SUFFIX ,
  CHAR_CLASS_OCT_DIGITS_INT_SUFFIX_DOT_X_E,
  CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX_E,
  CHAR_CLASS_DEC_DIGITS_INT_SUFFIX_DOT_E,
  CHAR_CLASS_OCT_DIGITS_INT_SUFFIX_DOT,
  CHAR_CLASS_HEX_DIGITS_INT_SUFFIX_DOT, 

  /* operator */
  CHAR_CLASS_SLASH,
  CHAR_CLASS_STAR ,
  CHAR_CLASS_TILDE,
  CHAR_CLASS_EXCLAIM ,
  CHAR_CLASS_PERCENT ,
  CHAR_CLASS_CARET ,
  CHAR_CLASS_AMPERSAND ,
  CHAR_CLASS_VERTICAL_BAR ,
  CHAR_CLASS_POSITIVE ,
  CHAR_CLASS_NEGATIVE ,

  CHAR_CLASS_AMPERSAND_EQUAL ,
  CHAR_CLASS_VERTICAL_BAR_EQUAL ,
  CHAR_CLASS_POSITIVE_EQUAL ,
  CHAR_CLASS_NEGATIVE_EQUAL ,
  CHAR_CLASS_NEGATIVE_EQUAL_GREATER,


  CHAR_CLASS_LESS ,
  CHAR_CLASS_GREATER ,
  CHAR_CLASS_LESS_EQUAL ,
  CHAR_CLASS_GREATER_EQUAL ,
  CHAR_CLASS_SLASH_STAR_EQUAL,
  CHAR_CLASS_EQUAL, 
  CHAR_CLASS_STAR_SLASH,

  /* char and string */
  CHAR_CLASS_DOUBLE_QUOTE_NEWLINE_BACKSLASH,
  CHAR_CLASS_BACKSLASH,
  CHAR_CLASS_SINGLE_QUOTE_NEWLINE_BACKSLASH,
  CHAR_CLASS_DOUBLE_QUOTE_NEWLINE,
  CHAR_CLASS_SINGLE_QUOTE_BACKSLASH_NEWLINE,
  CHAR_CLASS_SINGLE_QUOTE,
  CHAR_CLASS_DOUBLE_QUOTE,
  CHAR_CLASS_DOUBLE_QUOTE_SINGLE_QUOTE,
  CHAR_CLASS_SINGLE_QUOTE_NEWLINE,
  _CHAR_CLASS_NULL,

} char_class;

extern char *char_class2string[];
int  char_is_in_class(entry_t cond, entry_t meet);
bool is_white_space(char);

#endif

