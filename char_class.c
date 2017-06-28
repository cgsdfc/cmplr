#include "char_class.h"

bool is_white_space(char ch)
{
  return char_is_in_class(CHAR_CLASS_SPACES, ch);
}

// TODO too much , messy, clear up
char *char_class2string[]=
{
  [CHAR_CLASS_EMPTY]="",

  /* number */
  [CHAR_CLASS_DOT]=".",
  [CHAR_CLASS_ZERO]="0",
  [CHAR_CLASS_DEC_DIGITS]="0123456789",
  [CHAR_CLASS_LETTER]=CC_UPPER CC_LOWER,
  [ CHAR_CLASS_E ]= "eE",
  [CHAR_CLASS_DEC_DIGITS_LETTERS]=CC_DEC_DIGITS CC_UPPER CC_LOWER,
  [ CHAR_CLASS_DEC_DIGITS_LETTERS_DOT ]=CC_DOT  CC_DEC_DIGITS CC_UPPER CC_LOWER, 
  [ CHAR_CLASS_SIGN ]="-+",
  [ CHAR_CLASS_XX]="xX",
  [CHAR_CLASS_OCT_DIGITS]="01234567",
  [ CHAR_CLASS_HEX_DIGITS]="0123456789abcdefABCDEF",
  [CHAR_CLASS_DEC_DIGITS_NON_ZERO]="123456789",
  [ CHAR_CLASS_FLOAT_SUFFIX]="fFlL",
  [ CHAR_CLASS_INT_SUFFIX]="lLuU",

  [ CHAR_CLASS_DEC_DIGITS_INT_SUFFIX ]=CC_DEC_DIGITS CC_INT_SUFFIX , /* [^dec, isf] */
  [ CHAR_CLASS_OCT_DIGITS_INT_SUFFIX ]=CC_OCT_DIGITS CC_INT_SUFFIX , /* [^oct, isf] */
  [ CHAR_CLASS_HEX_DIGITS_INT_SUFFIX ]=CC_HEX_DIGITS CC_INT_SUFFIX , /* [^hex, isf] */
  /* the rev class */
  [ CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX ]=CC_DEC_DIGITS CC_FLOAT_SUFFIX, /* [^dec,fsf] */
  [ CHAR_CLASS_DEC_DIGITS_INT_SUFFIX_DOT_E ]=CC_DEC_DIGITS CC_INT_SUFFIX CC_DOT "eE", /* [^dec, isf, .] */
  [ CHAR_CLASS_OCT_DIGITS_INT_SUFFIX_DOT ]=CC_OCT_DIGITS CC_INT_SUFFIX CC_DOT, /* [^oct, isf, .] */
  [ CHAR_CLASS_HEX_DIGITS_INT_SUFFIX_DOT ]=CC_HEX_DIGITS CC_INT_SUFFIX CC_DOT, /* [^hex isf, .] */
  /* for single 0 */
  /* [^oct, isf, ., xX] */
  [ CHAR_CLASS_OCT_DIGITS_INT_SUFFIX_DOT_X_E ]=CC_OCT_DIGITS CC_INT_SUFFIX CC_DOT "eExX",
  [ CHAR_CLASS_DEC_DIGITS_FLOAT_SUFFIX_E ]=CC_DEC_DIGITS CC_FLOAT_SUFFIX "eE",
  /* number */

  /* id */
  [CHAR_CLASS_IDENTIFIER_BEGIN]=
    "_" CC_UPPER CC_LOWER,
  [CHAR_CLASS_IDENTIFIER_PART] = CC_IDENTIFIER_BEGIN CC_DEC_DIGITS,
  /* id */


  [CHAR_CLASS_SPACES]=CC_SPACES,
  [CHAR_CLASS_PUNCTUATION_NON_DOT]=CC_PUNCTUATION_NON_DOT,


  /* operator */
  [CHAR_CLASS_SLASH]="/",
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
  [CHAR_CLASS_STAR_SLASH]="*/",


  [CHAR_CLASS_LESS]="<",
  [CHAR_CLASS_GREATER]=">",
  [CHAR_CLASS_LESS_EQUAL]="<=",
  [CHAR_CLASS_GREATER_EQUAL]=">=",
  [CHAR_CLASS_SLASH_STAR_EQUAL]="/*=",
  [CHAR_CLASS_EQUAL]="=",
  /* operator */

  /* string and char */ 
  [CHAR_CLASS_BACKSLASH]="\\",
  [CHAR_CLASS_SINGLE_QUOTE]="\'",
  [CHAR_CLASS_DOUBLE_QUOTE]="\"",
  [CHAR_CLASS_DOUBLE_QUOTE_NEWLINE_BACKSLASH]="\\\n\r\"",
  [CHAR_CLASS_SINGLE_QUOTE_NEWLINE_BACKSLASH]="\\\n\r\'",
  [CHAR_CLASS_DOUBLE_QUOTE_NEWLINE]="\n\r\"",
  [ CHAR_CLASS_SINGLE_QUOTE_BACKSLASH_NEWLINE ]="\'\\\n\r",
  [ CHAR_CLASS_SINGLE_QUOTE_NEWLINE ]="\'\n\r",
  [ CHAR_CLASS_DOUBLE_QUOTE_SINGLE_QUOTE ]="\'\"",
  [CHAR_CLASS_NEWLINE]="\n\r",
  /* string and char */ 



};

int char_is_in_class(entry_t cond, entry_t ch)
{
  assert (CHAR_CLASS_EMPTY <= cond && cond < _CHAR_CLASS_NULL);
  assert (ch); // '\0' does no good

  char *chcl=char_class2string[cond];
  assert (chcl);
  bool in_class = strchr(chcl, ch);
  return in_class;
}
