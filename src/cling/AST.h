#ifndef CLING_AST_HXX
#define CLING_AST_HXX
#include "Utility.h"
struct ConstDecl;
struct VarDecl;
struct FunctionDecl;
struct Type;
struct MutableType;
struct ImmutableType;
struct FunctionType;
struct ArrayType;
struct ConstType;
struct VarType;
struct ConstDef;
struct VarDef;

struct TokenValue;
struct Program {
  GenericVector<ConstDecl> *constDecls;
  GenericVector<VarDecl> *varDecls;
  GenericVector<FunctionDecl> *funcDecls;
  /*
   * We use template to save long type list
   */
  template<class Clist, class Vlist, class Flist>
  Program(Clist C, Vlist V, Flist F): constDecls(C), varDecls(V), funcDecls(F){}
  ~Program();
};

struct ConstDecl {
  ConstType const* type;
  GenericVector<ConstDef> *constDefs;
  template<class Tp, class C>
    ConstDecl(Tp type, C constDefs):type(type), constDefs(constDefs) {}
  ~ConstDecl();
};

struct VarDecl {
  Type const *type;
  GenericVector<VarDef> varDefs;
};

struct VarDef {
  TokenValue *iden;
  MutableType *type;
};

struct ConstDef {
  Type *type;
  TokenValue *iden;
  TokenValue *value;
  ConstDef(TokenValue *iden, TokenValue *value):iden(iden), value(value){}
  
};

struct FunctionDecl {
  FunctionType *type;
  TokenValue *iden;
};

struct Type {

};

struct MutableType: public Type {

};

struct ImmutableType: public Type {


};

struct ConstType: public ImmutableType {

};

struct ArrayType: public MutableType {

};

struct FunctionType: public ImmutableType {

};

struct VarType: public MutableType {

};

#endif
