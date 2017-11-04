#ifndef UTILLIB_LL1_H
#define UTILLIB_LL1_H
#include "vector.h"
#include "enum.h"
#include "rule.h"

/**
 * \struct utillib_ll1_builder
 * Builder of the LL(1) parser table.
 */
struct utillib_ll1_builder {
  struct utillib_ll1_first_set first_sets;
  struct utillib_ll1_first_set first_sets_p;
  struct utillib_vector follow_sets;
  struct utillib_rule_index * rule_index;
};

/**
 * \struct utillib_ll1_parser
 * Parser for the LL(1) grammar.
 * Handles the runtime parsing of input.
 */

struct utillib_ll1_parser {
 struct utillib_vector stack;
 struct const utillib_rule * table;
 struct utillib_abstract_scanner *scanner;
 void * client_data;
};

struct utillib_ll1_first_set {
  utillib_vector elements;
  bool has_epsilon;
};

void utillib_ll1_parser_init(struct utillib_ll1_parser *self);
int utillib_ll1_parser_parse(struct utillib_ll1_parser *self);

#endif // UTILLIB_LL1_H
