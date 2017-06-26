/* statement.c */
#include "statement.h"


// create construct like if (...), else if (...)
void add_keyword_parenthsis_expression (parser_state *states, token_type tt)
{
  add_initial(states[0],tt);
  add_intermedia(states[0], states[1], TKT_LEFT_PARENTHSIS);
  add_intermedia(states[1], states[2], NTT_EXPRESSION);
  add_intermedia(states[2], states[3], TKT_RIGHT_PARENTHSIS);
}


void init_for_statement(void)
{
  add_initial(PA_FOR_BEGIN,TKT_KW_FOR);
  add_intermedia(PA_FOR_BEGIN,PA_FOR_OPEN,TKT_LEFT_PARENTHSIS);
  add_intermedia(PA_FOR_HEAD,PA_FOR_INIT,NTT_STATEMENT);
  add_intermedia(PA_FOR_INIT,PA_FOR_COND,NTT_STATEMENT);
  add_intermedia(PA_FOR_COND,PA_FOR_INCR, NTT_STATEMENT);
  add_intermedia(PA_FOR_INCR,PA_FOR_CLOSE,TKT_RIGHT_PARENTHSIS);
  add_accepted(PA_FOR_CLOSE,NTT_STATEMENT);
}

void init_while_statement(void)
{
  const parser_state while_states[]=
  {
    PA_WHILE_BEGIN,
    PA_WHILE_OPEN,
    PA_WHILE_COND,
    PA_WHILE_CLOSE,
  };
  add_keyword_parenthsis_expression(while_states,TKT_KW_WHILE);
  add_accepted(PA_WHILE_CLOSE, PA_WHILE_END, NTT_STATEMENT);
}

void init_do_while_statement(void)
{
  const parser_state do_while_states[]=
  {
    PA_DO_WHILE_BEGIN,
    PA_DO_WHILE_OPEN,
    PA_DO_WHILE_COND,
    PA_DO_WHILE_CLOSE,
  };

  add_initial(PA_DO_WHILE_BEGIN,TKT_KW_DO);
  add_intermedia(PA_DO_WHILE_BEGIN, PA_DO_WHILE_BODY, NTT_STATEMENT);
  add_keyword_parenthsis_expression(do_while_states, TKT_KW_WHILE);
}

void init_if_statement(void)
{
  const parser_state if_states[]=
  {
    PA_IF_BEGIN,
    PA_IF_OPEN,
    PA_IF_COND,
    PA_IF_CLOSE,
  };

  add_keyword_parenthsis_expression(if_states,TKT_KW_IF);
  add_intermedia(PA_IF_BODY,PA_IF_CLOSE,NTT_STATEMENT);
  add_accepted_rev(PA_IF_BODY,PA_IF_END,TKT_ELSE);

  add_intermedia(PA_IF_BODY,PA_ELSE_BEGIN,TKT_KW_ELSE);
}

static
parser_state do_transfer(parser_state state, token_type tkt,  entry_t *entry)
{
  return st_do_transfer(parser_table, tkt, entry, PA_NULL);
}


int parse_if_statement(token_buffer *tkb, ast_node *pnode, parser_state *errstate)
{
  ast_if_node *if_node = malloc(sizeof(ast_if_node));
  bool is_accepted=false;
  token *tk=NULL;
  token_type tkt=TKT_UNKNOWN;
  parser_state state=init_st;
  entry_t entry;

  while (!is_accepted)
  {
    tk=get_token(tkb);
    tkt=TOKEN_TYPE(tk);
    state=do_transfer(state,tkt,&entry);
    if (state == PA_NULL)
    {

    }
  }
}

typedef int (*parser_action_func) (token_buffer*,ast_node**,parser_state*);

typedef struct parser_action
{
  token_type tkt;
  parser_action_func action;
} parser_action;

static const parser_action[]=
{
  { TKT_KW_DO, parse_do_while_statement },
  { TKT_KW_IF, parse_if_statement },
  { TKT_KW_FOR, parse_for_statement },
  { TKT_KW_WHILE, parse_while_statement },
  { TKT_KW_SWITCH, parse_switch_statement },
  { TKT_KW_BREAK, NULL },
  { TKT_KW_CONTINUE, NULL },
  { TKT_KW_GOTO, parse_goto_statement },
  { TKT_KW_RETURN, parse_return_statement },





int parse_statement(token_buffer *tkb, ast_node **pnode, parser_state *errstate)
{
  token *tk=get_token(tkb);
  token_type tkt=TOKEN_TYPE(tk);
  switch (tkt)
  {
    case TKT_SEMICOLON:
      // empty stmt
      *pnode=0;
      return 0;

    case TKT_KW_IF:
      if(parse_if_statement(tkb,pnode,errstate)!=0)
      {
        return -1;
      }
      return 0;

    case TKT_KW_WHILE:
      if (parse_while_statement(tkb,pnode,errstate)!=0)
      {
        return -1;
      }
      return 0;

    case TKT_KW_FOR:
      if (parse_for_statement(tkb,pnode,errstate)!=0)
      {
        return -1;
  }

}








