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

ConstDef::ConstDef(TokenValue *name, TokenValue *value)
  :name(static_cast<Identifier*>(name)),
  value(static_cast<IntegralToken*>(value)){}

ConstDef::~ConstDef() {
  delete name;
  delete value;
}

VarDef::VarDef(TokenValue *name, Type *type)
  :name(static_cast<Identifier*> (name)),
  type(type){}

VarDef::~VarDef() {
  delete name;
  delete type;
}

VarDecl::~VarDecl() {
  delete type;
  delete varDefs;
}

FunctionDecl::~FunctionDecl() {
  delete signature;
  delete name;
  delete statements;
}

FunctionType::~FunctionType() {
  delete returnType;
  delete argvTypes;
}

unsigned int Type::GetStorageSize() {
  return 0;
}

unsigned int ArrayType::GetStorageSize() {
  return extend * elementType->GetStorageSize();
}

bool FunctionType::SupportOperation(Operation const *operation) {
  switch(operation->GetKind()) {
    case OperationKind::CALL:
      return true;
    default:
      return false;
  }
}

bool ConstType::SupportOperation(Operation const *operation) {
  switch (operation->GetKind()) {
    case  OperationKind::ASSIGN:
    case OperationKind::SCANF:
      return true;
    default:
      return baseType->SupportOperation(operation);
  }
}

bool ArrayType::SupportOperation(Operation const *operation) {
  switch(operation->GetKind()) {
    case OperationKind::INDEX:
      return true;
    default:
      return false;
  }
}

BinaryExpression::~BinaryExpression() {
  delete operation;
  delete leftOperand;
  delete rightOperand;
}

UnaryExpression::~UnaryExpression() {
  delete operation;
  delete operand;
}

CallExpression::~CallExpression() {
  delete callee;
  delete args;
}

IfStatement::~IfStatement() {
  delete condition;
  delete thenClause;
  delete elseClause;
}

ForStatement::~ForStatement() {
  delete initialExpression;
  delete conditionExpression;
  delete stepExpression;
  delete clauseStatement;
}

CaseStatement::CaseStatement(TokenValue *c, Statement *s):
  constant(static_cast<IntegralToken*>(c)),
  statement(s){}

CaseStatement::~CaseStatement() {
  delete constant;
  delete statement;
}

SwitchStatement::~SwitchStatement() {
  delete selectiveExpression;
  delete caseClauses;
  delete defaultClause;
}

ReturnStatement::~ReturnStatement() { delete returnExpression; }

IndexExpression::~IndexExpression() {
  delete array;
  delete index;
}
