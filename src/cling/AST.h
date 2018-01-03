#ifndef CLING_AST_HXX
#define CLING_AST_HXX
#include <vector>
#include "Utility.h"
struct ConstDecl;
struct VarDecl;
struct Function;
struct Type;
struct ConstDef;
struct VarDef;

struct Program {
  GenericVector<ConstDecl> *constDecls;
  GenericVector<VarDecl> *varDecls;
  GenericVector<Function> *funcDecls;
  Program(): constDecls(nullptr), varDecls(nullptr), funcDecls(nullptr){}
};

struct ConstDecl {
  Type const* type;
  GenericVector<ConstDef> constDefs;
};

struct VarDecl {
  Type const *type;
  GenericVector<VarDef> varDefs;
};


#endif
