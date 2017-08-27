#include "token_type.h"
#include <string.h>

/** NOTICE: do not change this */
static const char *keywords_tab[] = {
  [TKT_KW_FOR] = "for",
  [TKT_KW_WHILE] = "while",
  [TKT_KW_IF] = "if",
  [TKT_KW_ELSE] = "else",
  [TKT_KW_SWITCH] = "switch",
  [TKT_KW_CASE] = "case",
  [TKT_KW_GOTO] = "goto",
  [TKT_KW_RETURN] = "return",


  [TKT_KW_AUTO] = "auto",
  [TKT_KW_DOUBLE] = "double",
  [TKT_KW_STRUCT] = "struct",
  [TKT_KW_BREAK] = "break",
  [TKT_KW_LONG] = "long",
  [TKT_KW_ENUM] = "enum",
  [TKT_KW_REGISTER] = "register",
  [TKT_KW_TYPEDEF] = "typedef",
  [TKT_KW_CHAR] = "char",
  [TKT_KW_EXTERN] = "extern",
  [TKT_KW_UNION] = "union",
  [TKT_KW_CONST] = "const",
  [TKT_KW_FLOAT] = "float",
  [TKT_KW_SHORT] = "short",
  [TKT_KW_UNSIGNED] = "unsigned",
  [TKT_KW_CONTINUE] = "continue",
  [TKT_KW_SIGNED] = "signed",
  [TKT_KW_VOID] = "void",
  [TKT_KW_DEFAULT] = "default",
  [TKT_KW_SIZEOF] = "sizeof",
  [TKT_KW_VOLATILE] = "volatile",
  [TKT_KW_DO] = "do",
  [TKT_KW_STATIC] = "static",
  [TKT_KW_INT] = "int",



  [TKT_BINARY_OP_DIV] = "/",
  [TKT_BINARY_OP_MOD] = "%",


  [TKT_BINARY_OP_MOD_ASSIGN] = "%",
  [TKT_BINARY_OP_ADD_ASSIGN] = "+=",
  [TKT_BINARY_OP_SUB_ASSIGN] = "-=",
  [TKT_BINARY_OP_DIV_ASSIGN] = "/=",
  [TKT_BINARY_OP_MUL_ASSIGN] = "*=",
  [TKT_BINARY_OP_MOD_ASSIGN] = "%=",
  [TKT_BINARY_OP_ASSIGN] = "=",

  [TKT_BINARY_OP_BIT_AND_ASSIGN] = "&=",
  [TKT_BINARY_OP_BIT_OR_ASSIGN] = "|=",
  [TKT_BINARY_OP_BIT_XOR_ASSIGN] = "^=",
  [TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN] = "<<=",
  [TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN] = ">>=",

  [TKT_BINARY_OP_BIT_OR] = "|",
  [TKT_BINARY_OP_BIT_XOR] = "^",
  [TKT_BINARY_OP_BIT_LEFT_SHIFT] = "<<",
  [TKT_BINARY_OP_BIT_RIGHT_SHIFT] = ">>",

  [TKT_BINARY_OP_LOGICAL_AND] = "&&",
  [TKT_BINARY_OP_LOGICAL_OR] = "||",

  [TKT_BINARY_OP_CMP_LESS] = "<",
  [TKT_BINARY_OP_CMP_LESS_EQUAL] = "<=",
  [TKT_BINARY_OP_CMP_GREATER] = ">",
  [TKT_BINARY_OP_CMP_GREATER_EQUAL] = ">=",

  [TKT_BINARY_OP_CMP_EQUAL] = "==",
  [TKT_BINARY_OP_CMP_NOT_EQUAL] = "!=",

  [TKT_BINARY_OP_MEMBER_ARROW] = "->",

  [TKT_UNARY_OP_LOGICAL_NOT] = "!",
  [TKT_UNARY_OP_BIT_NOT] = "~",
  [TKT_UNARY_OP_PLUS_PLUS] = "++",
  [TKT_UNARY_OP_MINUS_MINUS] = "--",
  [TKT_AMPERSAND] = "&",
  [TKT_STAR] = "*",
  [TKT_PLUS] = "+",
  [TKT_MINUS] = "-",

  [TKT_LEFT_PARENTHESIS] = "(",
  [TKT_RIGHT_PARENTHESIS] = ")",
  [TKT_LEFT_BRACKET] = "[",
  [TKT_RIGHT_BRACKET] = "]",
  [TKT_LEFT_BRACE] = "{",
  [TKT_RIGHT_BRACE] = "}",
  [TKT_COLON] = ":",
  [TKT_SEMICOLON] = ";",
  [TKT_COMMA] = ",",
  [TKT_DOT] = ".",
  [TKT_QUESTION] = "?",

};

int
lookup_special (int from, int to, char *string)
{
  for (int i = from; i <= to; ++i)
    {
      if (strcmp (keywords_tab[i], string) == 0)
	{
	  return i;
	}
    }
  return TKT_UNKNOWN;
}
