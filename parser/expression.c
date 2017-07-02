#include "expression.h"
#include "tknzr/dfa.h"
#include "tknzr/token_type.h"
#include "tknzr/token_buffer.h"

#define N_PRECEDENCE 10
#define PCD_MAX_COL 14
enum
{
  PCD_10=0,
  PCD_9,
  PCD_8,
  PCD_7,
  PCD_6,
  PCD_5,
  PCD_4,
  PCD_3,
  PCD_2,
  PCD_1,
};

enum 
{
  RTN_SUCCESS,
  RTN_UNSUCCESS,
  RTN_LETGO,
  RTN_ERROR,
};



const static int precedence[][PCD_MAX_COL]=
{
  [PCD_1]= { TKT_STAR, TKT_BINARY_OP_MOD, TKT_BINARY_OP_DIV, -1},
  [PCD_2]= { TKT_PLUS, TKT_MINUS, -1 },
  [PCD_3]= { TKT_BINARY_OP_BIT_LEFT_SHIFT, TKT_BINARY_OP_BIT_RIGHT_SHIFT, -1 },
  [PCD_4]= { TKT_BINARY_OP_BIT_AND, -1 },
  [PCD_5]= {TKT_BINARY_OP_BIT_XOR, -1},
  [PCD_6]= { TKT_BINARY_OP_BIT_OR, -1},
  [PCD_7]= 
  { 
    TKT_BINARY_OP_CMP_GREATER,
    TKT_BINARY_OP_CMP_LESS,
    TKT_BINARY_OP_CMP_GREATER_EQUAL,
    TKT_BINARY_OP_CMP_LESS_EQUAL,
    TKT_BINARY_OP_CMP_EQUAL,
    TKT_BINARY_OP_CMP_NOT_EQUAL,
    -1
  },
  [PCD_8]= { TKT_BINARY_OP_LOGICAL_AND, -1},
  [PCD_9]= { TKT_BINARY_OP_LOGICAL_OR, -1},
  [PCD_10]=
  {
    TKT_BINARY_OP_MOD_ASSIGN,
    TKT_BINARY_OP_ADD_ASSIGN,
    TKT_BINARY_OP_SUB_ASSIGN,
    TKT_BINARY_OP_DIV_ASSIGN,
    TKT_BINARY_OP_MUL_ASSIGN,
    TKT_BINARY_OP_ASSIGN,
    TKT_BINARY_OP_BIT_AND_ASSIGN,
    TKT_BINARY_OP_BIT_OR_ASSIGN,
    TKT_BINARY_OP_BIT_XOR_ASSIGN,
    TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN,
    TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN,
    -1
  }
};

/** if input is a valid operator
 * token, then search for it in 
 * the precedence row given by cond.
 * or input is a parser return value,
 * if it indicates error, return -1,
 * else match it against cond.
 * the diff of 2 cases is told by
 * entry->usrd.
 */
int cond_expr(dfa_state *entry, int input)
{
  int cond=entry->cond;
  if (entry->usrd) 
  {
    // this is an operator
    for (int i=0;; ++i)
    {
      int op=precedence[cond][i];
      if (op == -1)
      {
        return 0;
      }
      if (op == input)
      {
        return 1;
      }
    }
  }
  if (input == RTN_ERROR)
    return -1;
  return input == cond;
}



int init_expr(void)
{
  int term=alloc_state(true);
  int expr=alloc_state(true);
  int exprs[15];

  for (int i=1;i<=10;i++)
  {
    exprs[i]=alloc_state(true);
  }


}

enum 
{
  PRA_GET_TOKEN,
  PRA_PUT_TOKEN,
  PRA_DO_RECR,
  PRA_DO_RETN,
  PRA_DO_NOTHING,
};

void init_expression(void)
{
  config_from(EXPRESSION);
    config_usrd(RETN_CLASS);
      config_condition(RTN_LETGO);
        config_action(PRA_DO_RECR);
          add_to(ASSIGN_OPER);
  config_from(EXP_END);
    config_usrd(TOKEN_CLASS);
      config_condition(TKT_COMMA);
        add_to(EXPRESSION);
    config_usrd(TOKEN_CLASS_REV);
        add_to(EXP_END);
}


void init_primary(void)
{
  config_state_action(PRIMARY, PRA_LOOKAHEAD);
  config_from(PRIMARY);
    config_usrd();
    config_condion(TKT_IDENTIFIER);
      config_action(PRA_CONSUME);
        add_to(EXP_END);
      config_condion(TKT_LEFT_PARENTHESIS);
        add_to(EXPRESSION);
}

void init_postfix(void)
{
  config_from(POSTFIX);
    config_action(PRA_GET_TOKEN);
      config_condition(TKT_LEFT_BRACKET);
        add_to(EXPRESSION);
      config_condition(TKT_DOT);
        add_to(POSTFIX_IDFR);
      config_condition(TKT_BINARY_OP_MEMBER_ARROW);
        add_to(POSTFIX_IDFR);
      config_condition(TKT_UNARY_OP_PLUS_PLUS);
        add_to(POSTFIX);
      config_condition(TKT_UNARY_OP_MINUS_MINUS);
        add_to(POSTFIX);
      config_condition(TKT_LEFT_PARENTHESIS);
        add_to(ARGLIST);

      config_from(POSTFIX_IDFR);
        config_condition(TKT_IDENTIFIER);
          add_to(POSTFIX);
    config_action(PRA_PUT_TOKEN);
      config_default();
        add_to(PRIMARY);
}
      
void init_arglist(void)
{
  config_from(ARG_LIST);
    config_action(PRA_DO_RECR);
      config_usrd(RETN_CLASS);
        config_condition(RTN_LETGO); // always return true, 
          add_to(ASSIGN_OPER);

  config_from(EXP_END);
    config_action(PRA_GET_TOKEN);
      config_usrd(TOKEN_CLASS);
        config_condition(TKT_COMMA);
          add_to(ARG_LIST);
        config_default();
          add_to(ARG_LIST_END);

  config_from(ARG_LIST_END);
    config_action(PRA_GET_TOKEN);
      config_condition(TKT_RIGHT_PARENTHESIS);
        add_to(EXP_END);
    
}

void init_unary(void)
{
  config_from(UNARY);
    config_action(PRA_GET_TOKEN);
      config_usrd(TOKEN_CLASS);
        config_condion(TKT_UNARY_OP_PLUS_PLUS);
          add_to(UNARY);
        config_condition(TKT_UNARY_OP_MINUS_MINUS);
          add_to(UNARY);
        config_condition(TKT_KW_SIZEOF);
          add_to(UNARY_SIZEOF);
        config_usrd(OPER_CLASS);
          config_condition(UNARY_OPERATORS);
            add_to(CAST);
        config_default();
          config_action(PRA_PUT_TOKEN);
            add_to(POSTFIX);
  config_from(UNARY_SIZEOF);
    config_usrd(TOKEN_CLASS);
      config_action(PRA_GET_TOKEN);
        config_condition(TKT_LEFT_PARENTHESIS);
          add_to(TYPE_NAME);
        config_default();
          add_to(UNARY);

}

void init_type_name(void)
{
  config_from(TYPE_NAME);
    config_action(PRA_GET_TOKEN);
      config_condition(TKT_RIGHT_PARENTHESIS);
        add_to(EXP_END);
}


void init_cast(void)
{
  config_from(CAST);
    config_action(PRA_GET_TOKEN);
        config_usrd(TOKEN_CLASS);
          config_condition(TKT_LEFT_PARENTHESIS);
            add_to(TYPE_NAME);
        config_from(EXP_END);
          config_condition(TKT_RIGHT_PARENTHESIS);
            add_to(CAST);
}

void init_mul_operator(void)
{
  config_from(MUL);
    config_condition(RTN_LETGO);
      config_action(PRA_DO_RECR);
        add_to(CAST);
  config_from(EXP_END);
    config_action(PRA_GET_TOKEN);
      config_usrd(OPER_CLASS);
        config_condition(MUL_OPERATIORS);
          add_to(MUL);
      config_usrd(OPER_CLASS_REV);
        config_action(PRA_PUT_TOKEN);
          add_to(EXP_END);
}

void init_add_operator(void)
{
  config_from(ADD);
    config_condition(RTN_LETGO);
      config_action(PRA_DO_RECR);
        add_to(MUL);
  config_from(EXP_END);
    config_action(PRA_GET_TOKEN);
      config_usrd(OPER_CLASS);
        config_condition(ADD_OPERATORS);
          add_to(ADD);
      config_usrd(OPER_CLASS_REV);
        config_action(PRA_PUT_TOKEN);
          add_to(EXP_END);
}


    



int parse_postfix(int state, token_buffer *buf)
{
  int cond=RTN_LETGO;
  dfa_state *entry;

  while (true)
  {
    r=transfer(table, state, cond, &entry);
    switch (entry->action)
    {
      case PRA_DO_RECR:
        cond=parse_postfix(entry->state, buf);
        break;

      case PRA_GET_TOKEN:
        get_token(buf, &tk);
        cond=tk->type;
        break;

      case PRA_PUT_TOKEN:
        put_token(buf);
        break;

      case PRA_DO_RETN:
        return cond;
    }
  }
}


#ifndef TOKEN_TYPE_H 
#define TOKEN_TYPE_H 1

#define N_KEYWORDS ( 32 )
#define FIRST_KW TKT_KW_FOR 
#define LAST_KW  TKT_KW_SIZEOF
#define FIRST_PUNC TKT_LEFT_PARENTHESIS
#define LAST_PUNC TKT_QUESTION
#define FIRST_OPER  TKT_BINARY_OP_DIV
#define LAST_OPER TKT_MINUS



int lookup_special(int from, int to, char *string);
typedef enum token_type 
{

  TKT_KW_FOR=0,
  TKT_KW_WHILE,
  TKT_KW_CASE,
  TKT_KW_RETURN,
  TKT_KW_SWITCH,
  TKT_KW_IF,
  TKT_KW_DEFAULT,
  TKT_KW_CONTINUE,
  TKT_KW_ELSE,
  TKT_KW_DO,
  TKT_KW_GOTO,
  TKT_KW_BREAK,

  /* qualifiers */
  TKT_KW_CONST,
  TKT_KW_VOLATILE,
  

  /* specifier */
  TKT_KW_DOUBLE,
  TKT_KW_SIGNED,
  TKT_KW_INT,
  TKT_KW_LONG,
  TKT_KW_CHAR,
  TKT_KW_FLOAT,
  TKT_KW_SHORT,
  TKT_KW_UNSIGNED,
  TKT_KW_VOID,
  /* compose type */
  TKT_KW_STRUCT,
  TKT_KW_ENUM,
  TKT_KW_UNION,

  /* storage */ 
  // can has at most one
  TKT_KW_TYPEDEF,
  TKT_KW_EXTERN,
  TKT_KW_STATIC,
  TKT_KW_AUTO,
  TKT_KW_REGISTER,

  /* sizeof operator */
  TKT_KW_SIZEOF,


  TKT_BINARY_OP_DIV,
  TKT_BINARY_OP_MOD,

  /* binary arithmetic assignment operator */
  TKT_BINARY_OP_MOD_ASSIGN,
  TKT_BINARY_OP_ADD_ASSIGN,
  TKT_BINARY_OP_SUB_ASSIGN,
  TKT_BINARY_OP_DIV_ASSIGN,
  TKT_BINARY_OP_MUL_ASSIGN,
  TKT_BINARY_OP_ASSIGN,

  /* binary bitwise assignment operator */
  TKT_BINARY_OP_BIT_AND_ASSIGN,
  TKT_BINARY_OP_BIT_OR_ASSIGN,
  TKT_BINARY_OP_BIT_XOR_ASSIGN,
  TKT_BINARY_OP_BIT_LEFT_SHIFT_ASSIGN,
  TKT_BINARY_OP_BIT_RIGHT_SHIFT_ASSIGN,

  /* binary bitwise operator */
  TKT_BINARY_OP_BIT_OR,
  TKT_BINARY_OP_BIT_XOR,
  TKT_BINARY_OP_BIT_LEFT_SHIFT,
  TKT_BINARY_OP_BIT_RIGHT_SHIFT,

  /* binary logical operator */
  TKT_BINARY_OP_LOGICAL_AND,
  TKT_BINARY_OP_LOGICAL_OR,

  /* binary comparation operator */
  TKT_BINARY_OP_CMP_LESS,
  TKT_BINARY_OP_CMP_LESS_EQUAL,
  TKT_BINARY_OP_CMP_GREATER,
  TKT_BINARY_OP_CMP_GREATER_EQUAL,

  /* binary comparation operator */
  TKT_BINARY_OP_CMP_EQUAL,
  TKT_BINARY_OP_CMP_NOT_EQUAL,
  TKT_BINARY_OP_MEMBER_ARROW,


  /* UNARY OPERATORS */
  TKT_UNARY_OP_LOGICAL_NOT,
  TKT_UNARY_OP_BIT_NOT,
  TKT_UNARY_OP_PLUS_PLUS,
  TKT_UNARY_OP_MINUS_MINUS,
  
  TKT_AMPERSAND,
  TKT_STAR,
  TKT_PLUS,
  TKT_MINUS,

  /* PUNCTUATIONS */
  TKT_LEFT_PARENTHESIS,
  TKT_RIGHT_PARENTHESIS,
  TKT_LEFT_BRACKET,
  TKT_RIGHT_BRACKET,
  TKT_LEFT_BRACE,
  TKT_RIGHT_BRACE,
  TKT_COLON,
  TKT_SEMICOLON,
  TKT_COMMA,
  TKT_DOT,
  TKT_QUESTION,
  _TKT_END,

} token_type;


enum
{
  TKT_INT_CONST=_TKT_END,
  TKT_BINARY_OP_MUL,
  TKT_FLOAT_CONST,
  TKT_CHAR_CONST,
  TKT_STRING_CONST,
  TKT_IDENTIFIER,
  TKT_BINARY_OP_MEMBER_DOT,
  TKT_BINARY_OP_ADD,
  TKT_BINARY_OP_SUB,

  TKT_UNARY_OP_NEGATIVE,
  TKT_UNARY_OP_ADDRESS,
  TKT_UNARY_OP_DEREFERENCE,
  TKT_BINARY_OP_BIT_AND,
  TKT_UNKNOWN,

};

#endif

