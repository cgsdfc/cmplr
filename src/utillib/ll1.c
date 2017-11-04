#include "ll1.h"

void utillib_ll1_parser_init(struct utillib_ll1_parser *self,
    struct utillib_ll1_parser_table * table)
{
  utillib_vector_init(&self->stack);
  utillib_vector_push_back(&self->stack, UTILLIB_SYMBOL_EOF);
  self->table=table;
}

int utillib_ll1_parser_parse(struct utillib_ll1_parser *self, struct utillib_abstract_scanner * scanner)
{
  while (true) {
    int sym=utillib_abstract_scanner_get_current_symbol(scan);

  }

}

void utillib_ll1_parser_destroy(struct utillib_ll1_parser *self) 
{
}

void utillib_ll1_builder_init(struct utillib_ll1_builder *self, 
   struct utillib_symbol const * symbols,
   struct utillib_rule const * rules) 
{
  utillib_vector_init(&self->first_sets);
  utillib_vector_init(&self->follow_sets);
  utillib_vector_init(&self->ll1_rules);
  size_t max_value=0;
  for (struct utillib_rule const * rule=rules; UTILLIB_RULE_LHS(rule)!=NULL;++rule) {
    utillib_vector_push_back(&self->ll1_rules, rule);
    struct utillib_symbol const * LHS=UTILLIB_RULE_LHS(rule);
    if (LHS->value > max_value) 
      max_value=LHS->value;
    for (struct utillib_symbol const * symbol=UTILLIB_RULE_RHS(rule);
        symbol!=NULL; ++symbol) {
      if (symbol->value > max_value)
        max_value=symbol->value;
    }
  }
}

static void ll1_builder_first_sets_terminal_init(struct utillib_ll1_builder *self) 
{
  for (size_t i=0, size=utillib_vector_size(&self->symbols);i<size;++i) {
    struct utillib_symbol const * symbol=&self->symbols[i];
    if (symbol->kind == UT_SYMBOL_TERMINAL) {
      struct utillib_vector * first_set=utillib_vector_at(&self->first_sets, i);
      utillib_vector_push_back(first_set, symbol);
    }
  }
}


static void ll1_builder_first_sets_loop(struct utillib_ll1_builder *self)
{

}

static void ll1_builder_make_table(struct utillib_ll1_builder *self)
{
  for (struct utillib_rule const * rule=self->productions;
      UTILLIB_RULE_LHS(rule) != NULL; ++rule) {
    size_t index=rule-self->productions;
    struct utillib_vector  const * first_set=
      utillib_vector_at(&self->first_sets_p, index);
    UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, first_set) {
      struct utillib_symbol const * LHS=UTILLIB_RULE_LHS(rule);
      if (terminal == UTILLIB_SYMBOL_EPSILON) {
        struct utillib_vector const * follow_set=
          utillib_vector_at(&self->follow_sets, index);
        UTILLIB_VECTOR_FOREACH(struct utillib_symbol const *, symbol, follow_set) {
          self->table[LHS->value][symbol->value]=UTILLIB_RULE_NULL;
        }
        continue;
      }
      self->table[LHS->value][symbol->value]=rule;
    }
  }
}
