#include "AST.h"

Program::~Program() {
  delete constDecls;
  delete varDecls;
  delete funcDecls;
}

ConstDecl::~ConstDecl() {
  delete type;
  delete constDefs;
}
