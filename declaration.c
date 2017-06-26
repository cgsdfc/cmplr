#include "declaration.h"



enum
{
  QUALIFIER=0,
  SPECIFIER=1,
  STORAGE=2,
};

static state_table *decl_table;
static bool qst_seen[3];
static type_qualifier qualifier;
static type_specifier specifier;
static storage_class storage;

bool decl_cond(entry_t condition, char tkt)
{
  if (is_terminal(tkt)) 
  {
    return condition == tkt;
  }

  switch (condition) {
    case NTT_TYPE_QUALIFIER:
      return  parse_type_qualifier(tkt, &qualifier);
    case NTT_TYPE_SPECIFIER:
      return  parse_type_specifier(tkt,&specifier);
    case NTT_STORAGE_CLASS:
      return parse_storage_class(tkt,&storage);
    default:
      return false;
  }

}
void init_declaration(void)
{
  decl_table=alloc_table();
  init_state_table(decl_table,
      "declaration's table",
      N_DECL_TABLE_ROWS,
      N_DECL_TABLE_COLS,
      DECL_NULL,
      decl_cond);

  add_initial(DECL_STORAGE_CLASS,NTT_STORAGE_CLASS);
  add_initial(DECL_TYPE_QUALIFIER,NTT_TYPE_QUALIFIER);
  add_initial(DECL_TYPE_SPECIFIER,NTT_TYPE_SPECIFIER);

  add_intermedia(DECL_STORAGE_CLASS,DECL_TYPE_QUALIFIER,NTT_TYPE_QUALIFIER);
  add_intermedia(DECL_STORAGE_CLASS,DECL_TYPE_SPECIFIER,NTT_TYPE_SPECIFIER);

  add_intermedia(DECL_TYPE_SPECIFIER,DECL_TYPE_QUALIFIER,NTT_TYPE_QUALIFIER);
  add_intermedia(DECL_TYPE_SPECIFIER,DECL_STORAGE_CLASS,NTT_STORAGE_CLASS);

  add_intermedia(DECL_TYPE_QUALIFIER,DECL_TYPE_SPECIFIER,NTT_TYPE_SPECIFIER);
  add_intermedia(DECL_TYPE_QUALIFIER,DECL_STORAGE_CLASS,NTT_STORAGE_CLASS);

  add_intermedia(DECL_STORAGE_CLASS,DECL_DECLARATOR_BEGIN,TKT_IDENTIFIER);
  add_intermedia(DECL_TYPE_QUALIFIER,DECL_DECLARATOR_BEGIN,TKT_IDENTIFIER);
  add_intermedia(DECL_TYPE_SPECIFIER,DECL_DECLARATOR_BEGIN,TKT_IDENTIFIER);

  add_accepted(DECL_DECLARATOR_BEGIN,DECL_DECLARATOR_END,TKT_SEMICOLON);
}

  
int parse_declaration(token_buffer *buf, ast_node **node, entry_t *errstate)
{





}



#ifndef DECLARATION_H 
#define DECLARATION_H 1

typedef enum declaration_state
{
  DECL_INIT,
  DECL_STORAGE_CLASS,
  DECL_TYPE_QUALIFIER,
  DECL_TYPE_SPECIFIER,
  DECL_DECLARATOR_BEGIN,
  DECL_DECLARATOR_END,
} declaration_state;



#endif


