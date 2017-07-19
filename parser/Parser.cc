#include "grammar.h"
#include "rules.h"
#include "item.h"
#include "stmt.h"

int lookahead(void)
{
  return 0;
}

int consume(void)
{
  return 0;
}

int parse_loop(parse_table *table, grammar *gr)
{
  int symbol_stack[1024]={0};
  int state_stack[1024]={0};
  int symbol_top=0;
  int state_top=0;
  int state=0;
  int next_state=0;
  int lhs=0;
  int symbol;
  int action=0;
  int ruleid=0;
  rule *r=0;

  state_stack[state_top++]=0;
  symbol=lookahead();

  while (true)
  {
    lookup_parse_table(table,state,symbol,&action,&next_state,&ruleid);
    state=state_stack[state_top];

    switch (action)
    {
      case PARSER_SHIFT:
        symbol_stack[symbol_top++]=symbol;
        consume();
        state_stack[state_top++]=next_state;
        break;

      case PARSER_REDUCE:
        r=gr->rules + ruleid;
        symbol=r->lhs;
        for (int i=0;i<r->len;++i)
        {
          symbol_top--;
        }
        symbol_stack[symbol_top++]=symbol;

        --state_top;
        state=state_stack[state_top-1];
        lookup_parse_table(table,state, symbol, NULL, &next_state, NULL);
        state_stack[state_top++]=next_state;
        break;

      case PARSER_ACCEPT:
        return 0;

      case PARSER_ERROR:
        return 1;
    }
  }

}





int main(int argc, char **argv)
{
  grammar gr;
  init_enum(&gr);
  show_rules(&gr);
  print_item_set_all(&gr);
  print_core(&gr);
}
