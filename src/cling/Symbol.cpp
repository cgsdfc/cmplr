#include "Symbol.h"
#include "Utility.h"

char const *SymbolString(int symbol) {
  switch(symbol) {
    case SYM_PROGRAM: return  "program";
    case SYM_ADD_OP: return  "additive operator";
    case SYM_REL_OP: return  "relational operator";
    case SYM_MUL_OP: return  "multiplicative operator";
    case SYM_CONSTANT: return  "constant";
    case SYM_TYPENAME: return  "typename";
    case SYM_CONST_DEF: return  "constant declaration";
    case SYM_VAR_DECL: return  "variable declaration";
    case SYM_VAR_DEF: return  "variable definition";
    case SYM_CONST_DECL: return  "constant definition";
    case SYM_FUNC_DECL: return  "function declaration";
    case SYM_MAIN_DECL: return  "main declaration";
    case SYM_NARGS: return  "normal arguments";
    case SYM_AARGS: return  "actual arguments";
    case SYM_IF_STMT: return  "if statement";
    case SYM_FOR_STMT: return  "for statement";
    case SYM_FOR_INIT: return  "initilizing part of for statement";
    case SYM_FOR_STEP: return  "steping part of for statement";
    case SYM_STMT: return  "statement";
    case SYM_SWITCH_STMT: return  "switch statement";
    case SYM_COMP_STMT: return  "composite statement";
    case SYM_PRINTF_STMT: return  "printf statement";
    case SYM_SCANF_STMT: return  "scanf statement";
    case SYM_EXPR_STMT: return  "expression statement";
    case SYM_RETURN_STMT: return  "return statement";
    case SYM_INVOKE_STMT: return  "invoke statement";
    case SYM_ELSE_CLAUSE: return  "else clause";
    case SYM_DEFAULT_CLAUSE: return  "default clause";
    case SYM_CASE_CLAUSE: return  "case clause";
    case SYM_EXPR: return  "expression";
    case SYM_CONDITION: return  "condition";
    case SYM_INTEGER: return  "integer";
    case SYM_STRING: return  "string";
    case SYM_CHAR: return  "character";
    case SYM_KW_INT: return  "int";
    case SYM_KW_CHAR: return  "char";
    case SYM_KW_VOID: return  "void";
    case SYM_KW_CONST: return  "const";
    case SYM_KW_IF: return  "if";
    case SYM_KW_FOR: return  "for";
    case SYM_KW_SWITCH: return  "switch";
    case SYM_KW_MAIN: return  "main";
    case SYM_KW_ELSE: return  "else";
    case SYM_KW_CASE: return  "case";
    case SYM_KW_DEFAULT: return  "default";
    case SYM_KW_RETURN: return  "return";
    case SYM_KW_SCANF: return  "scanf";
    case SYM_KW_PRINTF: return  "printf";
    case SYM_COLON: return  ":";
    case SYM_DEQ: return  "==";
    case SYM_ADD: return  "+";
    case SYM_MUL: return  "*";
    case SYM_RB: return  "}";
    case SYM_NE: return  "!=";
    case SYM_SEMI: return  ";";
    case SYM_IDEN: return  "identifier";
    case SYM_EQ: return  "=";
    case SYM_LK: return  "[";
    case SYM_RK: return  "]";
    case SYM_LB: return  "{";
    case SYM_LT: return  "<";
    case SYM_LE: return  "<=";
    case SYM_GT: return  ">";
    case SYM_GE: return  ">=";
    case SYM_DIV: return  "/";
    case SYM_MINUS: return  "-";
    case SYM_COMMA: return ",";
    case SYM_LP: return  "(";
    case SYM_RP: return  ")";
    default: UNREACHABLE(symbol);
  }
}
