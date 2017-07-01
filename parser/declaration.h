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


