#include "parser_table.h"

int lookup_parse_table(parse_table *table, int state, int symbol, int *action,
                       int *next_state, int *ruleid) {
  int *trans;
  trans = table->trans[state][symbol];
  if (action) {
    *action = trans[PI_ACTION];
  }
  if (next_state) {
    *next_state = trans[PI_NEXT_STATE];
  }
  if (ruleid) {
    *ruleid = trans[PI_RULE_ID];
  }
  return 0;
}
void parser_table_add_edge(parse_table *pt, int from, int to, int symbol) {
  pt->trans[from][symbol][PI_NEXT_STATE] = to;
}
