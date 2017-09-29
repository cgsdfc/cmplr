#include "lexer/token_type.h"

// TODO relexer/move it after parser is 
// done
/** this tab is for debug purpose */
const char *token_tab[] = {
  /* KEYWORDS */
  [TKT_KW_IF] = "TKT_KW_IF",
  [TKT_KW_FOR] = "TKT_KW_FOR",
  [TKT_KW_ELSE] = "TKT_KW_ELSE",
  [TKT_KW_GOTO] = "TKT_KW_GOTO",
  [TKT_KW_CASE] = "TKT_KW_CASE",
  [TKT_KW_SWITCH] = "TKT_KW_SWITCH",
  [TKT_KW_RETURN] = "TKT_KW_RETURN",
  [TKT_IDENTIFIER] = "TKT_IDENTIFIER",
  [TKT_KW_WHILE] = "TKT_KW_WHILE",
  [TKT_KW_AUTO] = "TKT_KW_AUTO",
  [TKT_KW_DOUBLE] = "TKT_KW_DOUBLE",
  [TKT_KW_STRUCT] = "TKT_KW_STRUCT",
  [TKT_KW_BREAK] = "TKT_KW_BREAK",
  [TKT_KW_LONG] = "TKT_KW_LONG",
  [TKT_KW_ENUM] = "TKT_KW_ENUM",
  [TKT_KW_REGISTER] = "TKT_KW_REGISTER",
  [TKT_KW_TYPEDEF] = "TKT_KW_TYPEDEF",
  [TKT_KW_CHAR] = "TKT_KW_CHAR",
  [TKT_KW_EXTERN] = "TKT_KW_EXTERN",
  [TKT_KW_UNION] = "TKT_KW_UNION",
  [TKT_KW_CONST] = "TKT_KW_CONST",
  [TKT_KW_FLOAT] = "TKT_KW_FLOAT",
  [TKT_KW_SHORT] = "TKT_KW_SHORT",
  [TKT_KW_UNSIGNED] = "TKT_KW_UNSIGNED",
  [TKT_KW_CONTINUE] = "TKT_KW_CONTINUE",
  [TKT_KW_SIGNED] = "TKT_KW_SIGNED",
  [TKT_KW_VOID] = "TKT_KW_VOID",
  [TKT_KW_DEFAULT] = "TKT_KW_DEFAULT",
  [TKT_KW_SIZEOF] = "TKT_KW_SIZEOF",
  [TKT_KW_VOLATILE] = "TKT_KW_VOLATILE",
  [TKT_KW_DO] = "TKT_KW_DO",
  [TKT_KW_STATIC] = "TKT_KW_STATIC",
  [TKT_KW_INT] = "TKT_KW_INT",

  /* PUNCTUATION */
  [TKT_LEFT_PARENTHESIS] = "TKT_LEFT_PARENTHESIS",
  [TKT_RIGHT_PARENTHESIS] = "TKT_RIGHT_PARENTHESIS",
  [TKT_LEFT_BRACKET] = "TKT_LEFT_BRACKET",
  [TKT_RIGHT_BRACKET] = "TKT_RIGHT_BRACKET",
  [TKT_LEFT_BRACE] = "TKT_LEFT_BRACE",
  [TKT_RIGHT_BRACE] = "TKT_RIGHT_BRACE",
  [TKT_COLON] = "TKT_COLON",
  [TKT_SEMICOLON] = "TKT_SEMICOLON",
  [TKT_COMMA] = "TKT_COMMA",
  [TKT_DOT] = "TKT_DOT",
  [TKT_QUESTION] = "TKT_QUESTION",

  /* OPERATORS */
  [TKT_STAR] = "TKT_STAR",
  [TKT_PLUS] = "TKT_PLUS",
  [TKT_MINUS] = "TKT_MINUS",
  [TKT_AMPERSAND] = "TKT_AMPERSAND",

  /* BINARY OPERATORS */
  [TKT_BINARY_OP_ADD] = "TKT_BINARY_OP_ADD",	// +
  [TKT_BINARY_OP_SUB] = "TKT_BINARY_OP_SUB",	// -
  [TKT_BINARY_OP_DIV] = "TKT_BINARY_OP_DIV",	// /
  [TKT_BINARY_OP_MUL] = "TKT_BINARY_OP_MUL",	// *
  [TKT_BINARY_OP_MOD] = "TKT_BINARY_OP_MOD",	// %
  [TKT_BINARY_OP_MOD_ASSIGN] = "TKT_BINARY_OP_MOD_ASSIGN",	// %=
  [TKT_BINARY_OP_ADD_ASSIGN] = "TKT_BINARY_OP_ADD_ASSIGN",	// +=
  [TKT_BINARY_OP_SUB_ASSIGN] = "TKT_BINARY_OP_SUB_ASSIGN",	// -=
  [TKT_BINARY_OP_DIV_ASSIGN] = "TKT_BINARY_OP_DIV_ASSIGN",	// /=
  [TKT_BINARY_OP_MUL_ASSIGN] = "TKT_BINARY_OP_MUL_ASSIGN",	// *=


  [TKT_BINARY_OP_ASSIGN] = "TKT_BINARY_OP_ASSIGN",	// =

  [TKT_BINARY_OP_BIT_AND_ASSIGN] = "TKT_BINARY_OP_BIT_AND_ASSIGN",	// &=
  [TKT_BINARY_OP_BIT_OR_ASSIGN] = "TKT_BINARY_OP_BIT_OR_ASSIGN",	// |=
  [TKT_BINARY_OP_BIT_XOR_ASSIGN] = "TKT_BINARY_OP_BIT_XOR_ASSIGN",	// ^=
  [TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN] = "TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN",	// <<=
  [TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN] = "TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN",	// >>=


  [TKT_BINARY_OP_BIT_AND] = "TKT_BINARY_OP_BIT_AND",	// &
  [TKT_BINARY_OP_BIT_OR] = "TKT_BINARY_OP_BIT_OR",	// |
  [TKT_BINARY_OP_BIT_XOR] = "TKT_BINARY_OP_BIT_XOR",	// ^
  [TKT_BINARY_OP_BIT_LEFT_SHIFT] = "TKT_BINARY_OP_BIT_LEFT_SHIFT",	// >>
  [TKT_BINARY_OP_BIT_RIGHT_SHIFT] = "TKT_BINARY_OP_BIT_RIGHT_SHIFT",	// <<

  [TKT_BINARY_OP_LOGICAL_AND] = "TKT_BINARY_OP_LOGICAL_AND",	// && 
  [TKT_BINARY_OP_LOGICAL_OR] = "TKT_BINARY_OP_LOGICAL_OR",	// ||
  [TKT_BINARY_OP_CMP_LESS] = "TKT_BINARY_OP_CMP_LESS",	// <
  [TKT_BINARY_OP_CMP_LESS_EQUAL] = "TKT_BINARY_OP_CMP_LESS_EQUAL",	// <=
  [TKT_BINARY_OP_CMP_GREATER] = "TKT_BINARY_OP_CMP_GREATER",	// >
  [TKT_BINARY_OP_CMP_GREATER_EQUAL] = "TKT_BINARY_OP_CMP_GREATER_EQUAL",	// >=
  [TKT_BINARY_OP_CMP_EQUAL] = "TKT_BINARY_OP_CMP_EQUAL",	// == 
  [TKT_BINARY_OP_CMP_NOT_EQUAL] = "TKT_BINARY_OP_CMP_NOT_EQUAL",	// !=
  [TKT_BINARY_OP_MEMBER_DOT] = "TKT_BINARY_OP_MEMBER_DOT",	// a.c
  [TKT_BINARY_OP_MEMBER_ARROW] = "TKT_BINARY_OP_MEMBER_ARROW",	// a->c

  /* UNARY OPERATORS */
  [TKT_UNARY_OP_LOGICAL_NOT] = "TKT_UNARY_OP_LOGICAL_NOT",	// !
  [TKT_UNARY_OP_BIT_NOT] = "TKT_UNARY_OP_BIT_NOT",	//~a
  [TKT_UNARY_OP_PLUS_PLUS] = "TKT_UNARY_OP_PLUS_PLUS",	//++a
  [TKT_UNARY_OP_MINUS_MINUS] = "TKT_UNARY_OP_MINUS_MINUS",	//--a
  [TKT_UNARY_OP_NEGATIVE] = "TKT_UNARY_OP_NEGATIVE",	//-a
  [TKT_UNARY_OP_ADDRESS] = "TKT_UNARY_OP_ADDRESS",	// &a
  [TKT_UNARY_OP_DEREFERENCE] = "TKT_UNARY_OP_DEREFERENCE",	// *p
  [TKT_STMT_COMPOUND] = "TKT_STMT_COMPOUND",
  /* CONSTS */
  [TKT_INT_CONST] = "TKT_INTEGER_CONST",
  [TKT_CHAR_CONST] = "TKT_CHARACTER_CONST",
  [TKT_STRING_CONST] = "TKT_STRING_CONST",
  [TKT_FLOAT_CONST] = "TKT_FLOAT_CONST",
  [TKT_BINARY_OP_SUBSCRIPT] = "TKT_BINARY_OP_SUBSCRIPT",
  [TKT_BINARY_OP_INVOKE] = "TKT_BINARY_OP_INVOKE",
  [TKT_UNKNOWN] = "TKT_UNKNOWN"
};
