/*
 * Although the parser is a c file, we compile
 * it as cpp so all cpp construct can be use
 */

/*
 * Base class for all the token
 */
%token_type {TokenValue*}
/* %token_destructor { delete $$; } */
%extra_argument { Parser *parser}
%start_symbol program


/*
 * start_symbol program
 */
%type program {Program*}
program ::= const_decl_list(C) var_decl_list(V) function_decl_list(F). {
        parser->program=new Program(C, V, F);
}
program::=var_decl_list(V) function_decl_list(F).{
       parser->program=new Program(nullptr, V, F);
}
program::=const_decl_list(C) function_decl_list(F). {
       parser->program=new Program(C, nullptr, F);
}
program::=function_decl_list(F).{
       parser->program=new Program(nullptr, nullptr, F);
}
program::=.{
       parser->program=nullptr;
}

/*
 * All the optional things
 */
%type maybe_const_decl_list {GenericVector<ConstDecl>*}
%type maybe_var_decl_list {GenericVector<VarDecl>*}
%type maybe_function_decl_list {GenericVector<FunctionDecl>*}
%type maybe_statement_list {GenericVector<Statement>*}
%type maybe_expression {ExpressionStatement*}

maybe_const_decl_list(L)::=.{L=nullptr;}
maybe_var_decl_list(L)::=.{L=nullptr;}
maybe_statement_list(L)::=.{L=nullptr;}
maybe_const_decl_list(L)::=const_decl_list(R).{L=R;}
maybe_var_decl_list(L)::=var_decl_list(R).{L=R;}
maybe_statement_list(L)::=statement_list(R).{L=R;}
maybe_expression(E)::=.{E=nullptr;}
maybe_expression(L)::=expression(R).{L=R;}

/* 
 * ConstDecl
 */
%type const_decl {ConstDecl*}
%type const_decl_list {GenericVector<ConstDecl>*}
%type const_def_list {GenericVector<ConstDef>*}
%type iden_int_pair_list {GenericVector<ConstDef>*}
%type maybe_signed_int {IntegerLiteral*}
%type char_const_def_list {GenericVector<ConstDef>*}
%type iden_int_pair {ConstDef*}
%type iden_char_pair {ConstDef*}

const_decl_list(L) ::= const_decl(C) . {
                L=new GenericVector<ConstDecl>();
                L->PushBack(C);

}
/*
 * Will the lemon delete LHS if we don not link it?
 */
const_decl_list(LHS) ::= const_decl_list(RHS) const_decl(C) . {
RHS->PushBack(C);
LHS=RHS;
}
const_decl(C) ::= SYM_KW_CONST const_def_list(L) SYM_SEMI. {
C=new ConstDecl(nullptr, L);
}

const_def_list(L) ::= SYM_KW_CHAR iden_char_pair_list(I) . {
L=I;
}
const_def_list(L)  ::= SYM_KW_INT iden_int_pair_list(C). {
L=C;
}

maybe_signed_int(M)::=SYM_ADD SYM_INTEGER(I).{
M=static_cast<IntegerLiteral*> (I);
}
maybe_signed_int(M)::=SYM_MINUS SYM_INTEGER(I).{
M=static_cast<IntegerLiteral*> (I);
M->intValue=-M->intValue;
}
maybe_signed_int(M)::=SYM_INTEGER(I).{
M=static_cast<IntegerLiteral*> (I);
}

iden_int_pair(I)::= SYM_IDEN(ID) SYM_EQ maybe_signed_int(INT). {
I=new ConstDef(ID, INT);
}

iden_int_pair_list(L) ::= iden_int_pair(I). {
L=new GenericVector<ConstDef> ();
L->PushBack(I);
}
iden_int_pair_list(LHS) ::= iden_int_pair_list(RHS) SYM_COMMA iden_int_pair(P). {
RHS->PushBack(P);
LHS=RHS;
}

/*
 * Almost the same as int version
 */

%type iden_char_pair_list { GenericVector<ConstDef> *}

iden_char_pair_list(L) ::= iden_char_pair(C) . {
L=new GenericVector<ConstDef> ();
L->PushBack(C);
}
iden_char_pair_list(L) ::= iden_char_pair_list(R) SYM_COMMA iden_char_pair(C) .{
R->PushBack(C);
L=R;
}
iden_char_pair(C) ::= SYM_IDEN(ID) SYM_EQ SYM_CHAR(CHAR) . {
C=new ConstDef(ID, CHAR);
}

%type type_specifier {Type*}
type_specifier(T) ::= SYM_KW_INT. {
T=new IntType();
}
type_specifier(T) ::=SYM_KW_CHAR. {
T=new CharType();
}
type_specifier(T) ::=SYM_KW_VOID. {
T=new VoidType();
}

/*
 * VarDecl
 */
%type var_decl_list {GenericVector<VarDecl>*}
%type var_decl {VarDecl*}
%type var_def {VarDef*}
%type var_def_list {GenericVector<VarDef>*}

var_decl_list(L) ::=var_decl(V) .{
L=new GenericVector<VarDecl>();
L->PushBack(V);
}
var_decl_list(L) ::=var_decl_list(R) var_decl(V) . {
R->PushBack(V);
L=R;
}

var_decl(VD) ::= type_specifier(T) var_def_list(VF) SYM_SEMI. {
VD=new VarDecl(T, VF);
}


/*
 * Single Variable
 */
var_def(V) ::= SYM_IDEN(I) . {
V=new VarDef(nullptr, I);
}


/*
 * Array
 */
var_def(V) ::= SYM_IDEN(I) SYM_LK SYM_INTEGER SYM_RK. {
V=new VarDef(nullptr, I);
}

var_def_list(L) ::= var_def(V) . {
L=new GenericVector<VarDef> ();
L->PushBack(V);
}

var_def_list(L) ::= var_def_list(R) SYM_COMMA var_def(V) . {
R->PushBack(V);
L=R;
}


/*
 * FunctionDecl
 */

%type function_decl {FunctionDecl*}
%type function_decl_list {GenericVector<FunctionDecl>*}
function_decl::= type_specifier SYM_IDEN SYM_LP formal_arglist SYM_RP 
             SYM_LB maybe_const_decl_list maybe_var_decl_list maybe_statement_list SYM_RB. {

}
function_decl_list(L) ::=function_decl_list(R) function_decl(F) . {
R->PushBack(F);
L=R;
}
function_decl_list(L) ::=function_decl(F) .{
L=new GenericVector<FunctionDecl> ();
L->PushBack(F);
}

%type formal_arglist {GenericVector<Type> *}
%type formal_arg {Type*}

formal_arglist(L) ::=.{ L=nullptr; }
formal_arglist(L) ::=formal_arg(A) .{
L=new GenericVector<Type> ();
L->PushBack(A);
}
formal_arglist(L) ::=formal_arglist(R) SYM_COMMA formal_arg(A) . {
R->PushBack(A);
L=R;
}
formal_arg(A) ::=type_specifier(T) SYM_IDEN.{ A=T; }

/*
 * Statement
 */
%type statement {Statement*}
%type statement_list {GenericVector<Statement>*}
%type for_statement {Statement*}
%type if_statement {Statement*}
%type switch_statement {Statement*}
%type expression_statement {Statement*}

statement_list(L) ::=statement(S) .{
L=new GenericVector<Statement> ();
L->PushBack(S);
}

statement_list(L) ::=statement_list(R)  statement(S) . {
R->PushBack(S);
L=R;
}

statement(S) ::= return_statement(R). { S=R; }
statement(S) ::=expression_statement(E) .{ S=E; }
statement(S) ::=for_statement(F).{ S=F; }
statement(S) ::=switch_statement(W) .{ S=W; }
statement(S) ::=if_statement(I) .{ S=I; }
statement(S) ::=SYM_LB statement_list(L) SYM_RB. { S=new CompStatement(L); }
statement(S)::=SYM_LB SYM_RB.{ S=nullptr; }

%type return_statement {ReturnStatement*}
return_statement(R) ::= SYM_KW_RETURN maybe_expression(E) SYM_SEMI. {
R=new ReturnStatement(E);
}

expression_statement(S)::=maybe_expression(E) SYM_SEMI. { S=E; }

for_statement(F) ::=SYM_KW_FOR SYM_LP maybe_expression(Init) SYM_SEMI maybe_expression(Cond) SYM_SEMI maybe_expression(Step) SYM_RP statement(Clause). {
F=new ForStatement(Init, Cond, Step, Clause);
}

%type default_clause {Statement*}
%type case_clause_list {GenericVector<CaseStatement> *}

switch_statement(S) ::=SYM_KW_SWITCH SYM_LP expression(Expr) SYM_RP 
                      SYM_LB case_clause_list(Cases) default_clause(Default) SYM_RB.{
S=new SwitchStatement(Expr, Cases, Default);
}

%type case_clause { CaseStatement*}
case_clause_list(L) ::=case_clause(C) .{
  L=new GenericVector<CaseStatement> ();
  L->PushBack(C);                
}

case_clause_list(L) ::= case_clause_list(R) case_clause(C) .{
R->PushBack(C);
L=R;
}
case_clause(Case) ::=SYM_KW_CASE constant(C) SYM_COLON statement(S) .{
Case=new CaseStatement(C, S);
}
default_clause(D) ::=SYM_KW_DEFAULT SYM_COLON statement(S) .{
D=S;
}
constant(C) ::=SYM_INTEGER(V) .{
C=V;
}
constant(C) ::=SYM_CHAR(V) .{
C=V;
}
/* The precedence of the shorter form is determined by 
 * the first terminal of that rule
 */
%nonassoc SYM_KW_IF.
/* The precedence of the token to shift is itself
 */
%nonassoc SYM_KW_ELSE.

if_statement(S) ::=SYM_KW_IF SYM_LP expression(Cond) SYM_RP statement(Then) SYM_KW_ELSE statement(Else) .{
S=new IfStatement(Cond, Then, Else);
}

if_statement(S) ::=SYM_KW_IF SYM_LP expression(Cond) SYM_RP statement(Then) .{
S=new IfStatement(Cond, Then, nullptr);
}

%type expression {ExpressionStatement*}
%type actual_arglist {GenericVector<ExpressionStatement>*}

expression(E) ::=SYM_IDEN(T) . { E=new AtomicExpression(T); }
expression(E) ::=SYM_CHAR(T).{ E=new AtomicExpression(T);}
expression(E) ::=SYM_INTEGER(T).{ E=new AtomicExpression(T);}
expression(E) ::=SYM_STRING(T).{ E=new AtomicExpression(T);}
expression(E) ::=SYM_LP expression(P) SYM_RP. {
 E=new ExpressionInParethesis(P);
}

expression(E) ::=expression(L) SYM_ADD expression(R). { E=new BinaryExpression(nullptr, L, R); }
expression(E)::=expression(L) SYM_MINUS expression(R). {E=new BinaryExpression(nullptr, L, R); }
expression(E) ::=expression(L) SYM_GE expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E)::=expression(L) SYM_DIV expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E) ::=expression(L) SYM_MUL expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E)::=expression(L) SYM_DEQ expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E) ::=expression(L) SYM_EQ expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E)::=expression(L) SYM_LE expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E) ::=expression(L) SYM_LT expression(R). { E=new BinaryExpression(nullptr, L, R);}
expression(E)::=expression(L) SYM_NE expression(R). { E=new BinaryExpression(nullptr, L, R);}


expression(E) ::=SYM_IDEN(Callee) SYM_LP actual_arglist(Args) SYM_RP. {
  E=new CallExpression(Callee, Args);
}

actual_arglist(L)::=.{L=nullptr;}

actual_arglist(L) ::=expression(E) .{
  L=new GenericVector<ExpressionStatement> ();
L->PushBack(E);              
}

actual_arglist(L) ::=actual_arglist(R) SYM_COMMA expression(E) .{
  R->PushBack(E);
L=R;              
}

expression(E) ::=SYM_IDEN(A) SYM_LK expression(I) SYM_RK.{
          E=new IndexExpression(A, I);
}
expression(E) ::=SYM_MINUS expression(O) . [SYM_RIGHT_UNARY] {
E=new UnaryExpression(nullptr, O);
}
expression(E) ::=SYM_ADD expression(O) . [SYM_RIGHT_UNARY] {
E=new UnaryExpression(nullptr, O);
}

%nonassoc SYM_EQ.
%left SYM_DEQ SYM_LT SYM_LE SYM_GE SYM_GT SYM_NE.
%left SYM_ADD SYM_MINUS.
%left SYM_DIV SYM_MUL.
/* This is for unary operator */
%right SYM_RIGHT_UNARY .


