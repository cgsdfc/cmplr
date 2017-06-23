#include "char_class.h"
#include <stdbool.h>
#include<stdlib.h>
#include <assert.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>

#define CC_CHAR_LITERAL " `\"#@$" CC_IDENTIFIER_PART\
  CC_OPERATOR CC_PUNCTUATION

#define CC_SINGLE_ESCAPE_NON_ZERO "abfnrtv\\\'\"?"
#define CC_SINGLE_ESCAPE "0" CC_SINGLE_ESCAPE_NON_ZERO 
#define CC_HEX_LETTER "abcdefABCDEF"
#define CC_HEX_BEGIN CC_DEC_PART CC_HEX_LETTER
#define CC_OCT_BEGIN_NOT_ZERO "1234567"
#define CC_OCT_BEGIN "0" CC_OCT_BEGIN_NOT_ZERO
#define CC_SINGLE_QUOTE "\'"
#define CC_DOUBLE_QUOTE "\""

#define CC_LOWER_CASE "abcdefghijklmnopqrstuvwxyz"
#define CC_UPPER_CASE "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CC_DEC_BEGIN "123456789" 
#define CC_DEC_PART "0123456789" 
#define CC_IDENTIFIER_BEGIN "_" CC_LOWER_CASE CC_UPPER_CASE
#define CC_IDENTIFIER_PART CC_IDENTIFIER_BEGIN CC_DEC_PART
#define CC_SPACES " \t\n\r\f\v"
#define CC_NEWLINE "\n\r"
#define CC_PUNCTUATION "(){}[]:;,?"
#define CC_OPERATOR "~!%^&*-+<>=/|"
#define CC_BACKSLASH "\\"
#define CC_L "lL"
#define CC_U "uU"
#define CC_E "eE"
#define CC_X "x"
#define CC_ZERO "0"
#define CC_PERIOD "."
#define CC_SIGN "-+"

#define CC_SEPARATOR \
  CC_SPACES CC_PUNCTUATION CC_OPERATOR

char *char_class2string[]=
{
  [CHAR_CLASS_EMPTY]="",

  [CHAR_CLASS_LOWER_CASE]=CC_LOWER_CASE,
  [CHAR_CLASS_UPPER_CASE]=CC_UPPER_CASE,
  [CHAR_CLASS_DEC_BEGIN]=CC_DEC_BEGIN,
  [CHAR_CLASS_DEC_PART]=CC_DEC_PART,
  [CHAR_CLASS_IDENTIFIER_BEGIN]="_" CC_LOWER_CASE CC_UPPER_CASE,
  [CHAR_CLASS_IDENTIFIER_PART] = CC_IDENTIFIER_BEGIN CC_DEC_PART,
  [CHAR_CLASS_SPACES]=CC_SPACES,
  [CHAR_CLASS_NEWLINE]=CC_NEWLINE,
  [CHAR_CLASS_PUNCTUATION]=CC_PUNCTUATION,
  [CHAR_CLASS_OPERATOR]=CC_OPERATOR,
  [CHAR_CLASS_BACKSLASH]=CC_BACKSLASH,
  [CHAR_CLASS_SEPARATOR] = CC_SPACES CC_PUNCTUATION CC_OPERATOR,

  [CHAR_CLASS_CHAR_LITERAL]=CC_CHAR_LITERAL  ,
  [CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO ]=CC_SINGLE_ESCAPE_NON_ZERO ,
  [CHAR_CLASS_SINGLE_ESCAPE]=CC_SINGLE_ESCAPE  ,
  [CHAR_CLASS_HEX_LETTER]=CC_HEX_LETTER ,
  [CHAR_CLASS_HEX_BEGIN]=CC_HEX_BEGIN ,
  [CHAR_CLASS_OCT_BEGIN_NOT_ZERO ]=CC_OCT_BEGIN_NOT_ZERO ,
  [CHAR_CLASS_OCT_BEGIN ]=CC_OCT_BEGIN ,
  [CHAR_CLASS_SINGLE_QUOTE]=CC_SINGLE_QUOTE ,
  [CHAR_CLASS_DOUBLE_QUOTE]=CC_DOUBLE_QUOTE ,
  [CHAR_CLASS_L]=CC_L,
  [CHAR_CLASS_U]=CC_U,
  [CAHR_CLASS_SLASH]="/",
  [CHAR_CLASS_STAR]="*",

  [CHAR_CLASS_TILDE]="~",
  [CHAR_CLASS_EXCLAIM]="!",
  [CHAR_CLASS_PERCENT]="%",
  [CHAR_CLASS_CARET]="^",
  [CHAR_CLASS_AMPERSAND]="&",
  [CHAR_CLASS_VERTICAL_BAR]="|",
  [CHAR_CLASS_POSITIVE]="+",
  [CHAR_CLASS_NEGATIVE]="-",

  [CHAR_CLASS_AMPERSAND_EQUAL]="&=",
  [CHAR_CLASS_VERTICAL_BAR_EQUAL]="|=",
  [CHAR_CLASS_POSITIVE_EQUAL]="+=",
  [CHAR_CLASS_NEGATIVE_EQUAL]="-=",
  [CHAR_CLASS_NEGATIVE_EQUAL_GREATER ]="-=>",


  [CHAR_CLASS_LESS]="<",
  [CHAR_CLASS_GREATER]=">",
  [CHAR_CLASS_LESS_EQUAL]="<=",
  [CHAR_CLASS_GREATER_EQUAL]=">=",
  [CHAR_CLASS_SLASH_STAR_EQUAL]="/*=",
  [CHAR_CLASS_X]="x",
  [CHAR_CLASS_ZERO]="0",
  [CHAR_CLASS_EQUAL]="=",
  [CHAR_CLASS_DOUBLE_QUOTE_NEWLINE_BACKSLASH]="\\\n\r\"",
  [CHAR_CLASS_DOUBLE_QUOTE_NEWLINE]="\n\r\"",

  [ CHAR_CLASS_SINGLE_QUOTE_BACKSLASH_NEWLINE ]="\'\\\n\r",
  [ CHAR_CLASS_SINGLE_QUOTE_NEWLINE ]="\'\n\r",

  [CHAR_CLASS_OCT_BEGIN_X_U_L]=CC_OCT_BEGIN CC_L CC_X CC_U,
  [CHAR_CLASS_HEX_BEGIN_U_L]=CC_HEX_BEGIN CC_U CC_L,
  [CHAR_CLASS_OCT_BEGIN_U_L]=CC_OCT_BEGIN CC_U CC_L,
  [CHAR_CLASS_PERIOD]=CC_PERIOD,
  [ CHAR_CLASS_OCT_BEGIN_X_U_L_PERIOD ]=CC_OCT_BEGIN
    CC_L CC_X CC_U CC_PERIOD,

  [ CHAR_CLASS_DEC_PART_PERIOD ]=CC_DEC_PART CC_PERIOD,
  [ CHAR_CLASS_E ]= CC_E,
  [ CHAR_CLASS_SIGN ]=CC_SIGN,
  [CHAR_CLASS_STAR_SLASH]="*/",
};

void check_char_class(void)
{
  puts("check_char_class begin");
  for (int i=CHAR_CLASS_EMPTY;i<_CHAR_CLASS_NULL;++i)
  {
    assert (char_class2string[i]);
  }
  puts("check_char_class passed");
}

