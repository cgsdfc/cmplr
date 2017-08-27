#ifndef PARSER_TABLE_H
#define PARSER_TABLE_H 1

typedef enum parse_table_limit {
  PR_MAX_STATE = 5,
  PT_MAX_SYMBOL = 10,
  PT_MAX_FIELD = 5,

}

typedef enum parse_table_index {
  PI_ACTION,
  PI_NEXT_STATE,
  PI_RULE_ID,

} parse_table_index;

typedef struct parse_table {
  int trans[PT_MAX_STATE][PT_MAX_SYMBOL][PT_MAX_FIELD];
  int nstate;
  int nsymbol;

} parse_table;

typedef enum parse_action {
  PARSER_SHIFT,
  PARSER_ACCEPT,
  PARSER_REDUCE,
  PARSER_ERROR,
} parse_action;

#endif
