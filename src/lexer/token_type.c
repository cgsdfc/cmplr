#include "lexer/token_type.h"
UTILLIB_ETAB_BEGIN(token_type)
UTILLIB_ETAB_ELEM(TKT_KW_FOR)
UTILLIB_ETAB_ELEM(TKT_KW_WHILE)
UTILLIB_ETAB_ELEM(TKT_KW_CASE)
UTILLIB_ETAB_ELEM(TKT_KW_RETURN)
UTILLIB_ETAB_ELEM(TKT_KW_SWITCH)
UTILLIB_ETAB_ELEM(TKT_KW_IF)
UTILLIB_ETAB_ELEM(TKT_KW_DEFAULT)
UTILLIB_ETAB_ELEM(TKT_KW_CONTINUE)
UTILLIB_ETAB_ELEM(TKT_KW_ELSE)
UTILLIB_ETAB_ELEM(TKT_KW_DO)
UTILLIB_ETAB_ELEM(TKT_KW_GOTO)
UTILLIB_ETAB_ELEM(TKT_KW_BREAK)
UTILLIB_ETAB_ELEM(TKT_KW_CONST)
UTILLIB_ETAB_ELEM(TKT_KW_VOLATILE)
UTILLIB_ETAB_ELEM(TKT_KW_DOUBLE)
UTILLIB_ETAB_ELEM(TKT_KW_SIGNED)
UTILLIB_ETAB_ELEM(TKT_KW_INT)
UTILLIB_ETAB_ELEM(TKT_KW_LONG)
UTILLIB_ETAB_ELEM(TKT_KW_CHAR)
UTILLIB_ETAB_ELEM(TKT_KW_FLOAT)
UTILLIB_ETAB_ELEM(TKT_KW_SHORT)
UTILLIB_ETAB_ELEM(TKT_KW_UNSIGNED)
UTILLIB_ETAB_ELEM(TKT_KW_VOID)
UTILLIB_ETAB_ELEM(TKT_KW_STRUCT)
UTILLIB_ETAB_ELEM(TKT_KW_ENUM)
UTILLIB_ETAB_ELEM(TKT_KW_UNION)
UTILLIB_ETAB_ELEM(TKT_KW_TYPEDEF)
UTILLIB_ETAB_ELEM(TKT_KW_EXTERN)
UTILLIB_ETAB_ELEM(TKT_KW_STATIC)
UTILLIB_ETAB_ELEM(TKT_KW_AUTO)
UTILLIB_ETAB_ELEM(TKT_KW_REGISTER)
UTILLIB_ETAB_ELEM(TKT_KW_SIZEOF)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_DIV)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_MOD)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_MOD_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_ADD_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_SUB_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_DIV_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_MUL_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_AND_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_OR_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_XOR_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_OR)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_XOR)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_LEFT_SHIFT)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_RIGHT_SHIFT)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_LOGICAL_AND)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_LOGICAL_OR)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_CMP_LESS)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_CMP_LESS_EQUAL)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_CMP_GREATER)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_CMP_GREATER_EQUAL)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_CMP_EQUAL)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_CMP_NOT_EQUAL)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_MEMBER_ARROW)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_LOGICAL_NOT)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_BIT_NOT)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_PLUS_PLUS)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_MINUS_MINUS)
UTILLIB_ETAB_ELEM(TKT_AMPERSAND)
UTILLIB_ETAB_ELEM(TKT_STAR)
UTILLIB_ETAB_ELEM(TKT_PLUS)
UTILLIB_ETAB_ELEM(TKT_MINUS)
UTILLIB_ETAB_ELEM(TKT_LEFT_PARENTHESIS)
UTILLIB_ETAB_ELEM(TKT_RIGHT_PARENTHESIS)
UTILLIB_ETAB_ELEM(TKT_LEFT_BRACKET)
UTILLIB_ETAB_ELEM(TKT_RIGHT_BRACKET)
UTILLIB_ETAB_ELEM(TKT_LEFT_BRACE)
UTILLIB_ETAB_ELEM(TKT_RIGHT_BRACE)
UTILLIB_ETAB_ELEM(TKT_COLON)
UTILLIB_ETAB_ELEM(TKT_SEMICOLON)
UTILLIB_ETAB_ELEM(TKT_COMMA)
UTILLIB_ETAB_ELEM(TKT_DOT)
UTILLIB_ETAB_ELEM(TKT_QUESTION)
UTILLIB_ETAB_ELEM(TKT_INT_CONST)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_MUL)
UTILLIB_ETAB_ELEM(TKT_FLOAT_CONST)
UTILLIB_ETAB_ELEM(TKT_CHAR_CONST)
UTILLIB_ETAB_ELEM(TKT_STRING_CONST)
UTILLIB_ETAB_ELEM(TKT_IDENTIFIER)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_MEMBER_DOT)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_ADD)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_CAST)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_SUB)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_INVOKE)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_SUBSCRIPT)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_NEGATIVE)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_POSITIVE)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_ADDRESS)
UTILLIB_ETAB_ELEM(TKT_FUNCTION)
UTILLIB_ETAB_ELEM(TKT_DECLARE)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_DEREFERENCE)
UTILLIB_ETAB_ELEM(TKT_BINARY_OP_BIT_AND)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_SIZOF_EXPR)
UTILLIB_ETAB_ELEM(TKT_UNARY_OP_SIZOF_TYPENAME)
UTILLIB_ETAB_ELEM(TKT_STMT_COMPOUND)
UTILLIB_ETAB_ELEM(TKT_STMT_LABEL)
UTILLIB_ETAB_ELEM(TKT_EOF)
UTILLIB_ETAB_ELEM(TKT_UNKNOWN)
UTILLIB_ETAB_END(token_type)


