#ifndef CLING_AST_HXX
#define CLING_AST_HXX
#include "Utility.h"
#include "Token.h"
struct ConstDecl;
struct VarDecl;
struct FunctionDecl;
struct Type;
struct FunctionType;
struct ArrayType;
struct ConstType;
struct VarType;
struct ConstDef;
struct VarDef;
struct Statement;
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
  GenericVector<VarDef> *varDefs;
  template<class Tp, class V>
    VarDecl(Tp type, V varDefs):type(type), varDefs(varDefs){}
  ~VarDecl();
};

struct VarDef {
  Identifier *name;
  Type *type;
  VarDef(TokenValue *name, Type *type);
  ~VarDef();
};

struct ConstDef {
  Identifier *name;
  IntegralToken *value;
  ConstDef(TokenValue *name, TokenValue *value);
  ~ConstDef();
};

struct FunctionDecl {
  FunctionType *signature;
  TokenValue *name;
  GenericVector<Statement> *statements;
  template<class Sig, class Nn, class Sta>
    FunctionDecl(Sig signature, Nn name, Sta statements):
      signature(signature), name(name), statements(statements) {}
  ~FunctionDecl();
};

enum class OperationKind {
  ARITH, 
  CALL,
  INDEX,
  ASSIGN, 
  PRINTF,
  SCANF,
};

struct Operation {
  unsigned int code;
  Operation(unsigned int code):code(code){}
  virtual ~Operation(){}
  virtual OperationKind GetKind() const =0;
};

struct ArithmeticOperation: public Operation {
  ArithmeticOperation(unsigned int code):Operation(code){}
  ~ArithmeticOperation()=default;
  OperationKind GetKind() const  override {
    return OperationKind::ARITH;
  }
};

struct AssignOperation: public Operation {
  OperationKind GetKind() const  override {
    return OperationKind::ASSIGN;
  }
};

struct CallOperation: public Operation {
  OperationKind GetKind() const  override {
    return OperationKind::CALL;
  }
};

struct Type {
  virtual char *GetTypeName()=0;
  virtual unsigned int GetStorageSize()=0;
  virtual ~Type(){}

  /*
   * This has a default impl which always return 0
   */
  virtual bool SupportOperation(Operation const *operation)=0;

};


struct ConstType: public Type {
  Type *baseType;

  ConstType(Type *baseType):baseType(baseType){}
  ~ConstType() {
    delete baseType;
  }
  bool SupportOperation(Operation const *operation)override;
  unsigned int GetStorageSize() {
    return Type::GetStorageSize();
  }
};

struct ArrayType: public Type {
  Type *elementType;
  unsigned int extend;
  ArrayType(Type *elementType, unsigned int extend)
    :elementType(elementType), extend(extend){}
  ~ArrayType() {
    delete elementType;
  }
  unsigned int GetStorageSize()override;
  bool SupportOperation(Operation const *operation)override;
};

struct FunctionType: public Type {
  Type *returnType;
  GenericVector<Type> *argvTypes;
  template<class R, class A>
    FunctionType(R returnType, A argvTypes):
      returnType(returnType), argvTypes(argvTypes){}
  ~FunctionType();
  bool SupportOperation(Operation const *operation)override;
  unsigned int GetStorageSize() {
    return Type::GetStorageSize();
  }
};

struct VarType: public Type {
  Type *baseType;

  VarType(Type *baseType):baseType(baseType){}
  ~VarType() {
    delete baseType;
  }
  unsigned int GetStorageSize()override;
  bool SupportOperation(Operation const* operation) override;


};

struct ExpressionStatement;
struct Statement {
  virtual ~Statement(){}

};

struct CompStatement: public Statement {
  GenericVector<Statement> *statements;
  template <class S> CompStatement(S s) : statements(s) {}
  ~CompStatement()=default;
};

struct IterativeStatement: public Statement {};

struct SelectiveStatement: public Statement {};

struct JumpStatement: public Statement {};

struct ReturnStatement: public JumpStatement {
  ExpressionStatement *returnExpression;
  ReturnStatement(ExpressionStatement *expr) : returnExpression(expr) {}
  ~ReturnStatement();
};

struct CaseStatement: public Statement {
  IntegralToken *constant;
  Statement *statement;

  CaseStatement(TokenValue *c, Statement *s);
  ~CaseStatement();
};

struct SwitchStatement: public SelectiveStatement {
  ExpressionStatement *selectiveExpression;
  GenericVector<CaseStatement> *caseClauses;
  Statement *defaultClause;

  template<class Expr, class Cases, class Default>
  SwitchStatement(Expr expr, Cases cases, Default def):
    selectiveExpression(expr),
    caseClauses(cases),
    defaultClause(def){}
  ~SwitchStatement();

};

struct IfStatement: public SelectiveStatement {
  ExpressionStatement *condition;
  Statement *thenClause;
  Statement *elseClause;
  template<class C, class T, class E>
  IfStatement(C condition, T thenClause, E elseClause):
    condition(condition),
    thenClause(thenClause),
    elseClause(elseClause){}
  ~IfStatement();
};

struct ForStatement: public IterativeStatement {
  ExpressionStatement *initialExpression;
  ExpressionStatement *conditionExpression;
  ExpressionStatement *stepExpression;
  Statement *clauseStatement;

  template<class Init, class Cond, class Step, class Clause>
    ForStatement(Init init, Cond cond, Step step, Clause clause):
      initialExpression(init),
      conditionExpression(cond),
      stepExpression(step),
      clauseStatement(clause) {}
  ~ForStatement();
};


struct ExpressionStatement: public Statement {
  Type *valueType;
  ExpressionStatement():valueType(nullptr) {}
  virtual ~ExpressionStatement() {
    delete valueType;
  }
};

struct BinaryExpression: public ExpressionStatement {
  Operation *operation;
  ExpressionStatement *leftOperand;
  ExpressionStatement *rightOperand;

  template<class Op, class L, class R>
    BinaryExpression(Op op, L lhs, R rhs)
    :operation(op), leftOperand(lhs), rightOperand(rhs){}
  ~BinaryExpression();

};

struct UnaryExpression: public ExpressionStatement {
  Operation *operation;
  ExpressionStatement *operand;

  template<class Op, class Operand>
    UnaryExpression(Op op, Operand operand):operation(op), operand(operand){}
  ~UnaryExpression();
};

struct CallExpression: public ExpressionStatement {
  Identifier *callee;
  GenericVector<ExpressionStatement> *args;

  template <class C, class A>
  CallExpression(C callee, A args)
      : callee(static_cast<Identifier *>(callee)), args(args) {}
  ~CallExpression();
};

struct IndexExpression : public ExpressionStatement {
  Identifier *array;
  ExpressionStatement *index;

  template <class A, class I>
  IndexExpression(A array, I index)
      : array(static_cast<Identifier *>(array)), index(index) {}
  ~IndexExpression();
};

struct AtomicExpression: public ExpressionStatement {
  TokenValue *token;
  AtomicExpression(TokenValue *token):token(token){}
  ~AtomicExpression() {
    delete token;
  }
};

struct ExpressionInParethesis: public ExpressionStatement { };

#endif
