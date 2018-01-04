/*
 * Although the parser is a c file, we compile
 * it as cpp so all cpp construct can be use
 */

/*
 * Base class for all the token
 */
%token_type {TokenValue*}
%token_destructor { delete $$; }
%extra_argument { Parser *parser}
%start_symbol program


/*
 * start_symbol program
 */
%type program {Program*}
program ::= const_decl_list(C) var_decl_list(V) function_decl_list(F). {
        parser->program=new Program(C, V, F);
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
%type int_const_def_list {GenericVector<ConstDef>*}
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

const_def_list(L) ::= int_const_def_list(I). {
L=I;
}
const_def_list(L)  ::= char_const_def_list(C). {
L=C;
}

int_const_def_list(LHS) ::= SYM_KW_INT iden_int_pair_list(RHS) . {
LHS=RHS;
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
char_const_def_list ::= SYM_KW_CHAR iden_char_pair_list. {

}

iden_char_pair_list ::= iden_char_pair. {

}
iden_char_pair_list::= iden_char_pair_list SYM_COMMA iden_char_pair.{

}
iden_char_pair::= SYM_IDEN SYM_EQ SYM_CHAR . {

}

type_specifier::= SYM_KW_INT. {

}
type_specifier::=SYM_KW_CHAR. {

}
type_specifier::=SYM_KW_VOID. {

}

/*
 * VarDecl
 */
%type var_decl_list {GenericVector<VarDecl>*}
var_decl_list::=var_decl.{

}
var_decl_list::=var_decl_list var_decl. {

}

var_decl ::= type_specifier var_def_list SYM_SEMI. {

}


/*
 * Single Variable
 */
var_def::= SYM_IDEN. {

}


/*
 * Array
 */
var_def::= SYM_IDEN SYM_LK SYM_INTEGER SYM_RK. {


}

var_def_list ::= var_def. {

}

var_def_list ::= var_def_list SYM_COMMA var_def. {

}



/*
 * FunctionDecl
 */

%type function_decl {FunctionDecl*}
%type function_decl_list {GenericVector<FunctionDecl>*}
function_decl::= type_specifier SYM_IDEN SYM_LP formal_arglist SYM_RP 
             SYM_LB maybe_const_decl_list maybe_var_decl_list maybe_statement_list SYM_RB. {

}
function_decl_list::=function_decl_list function_decl. {

}
function_decl_list::=function_decl.{

}

formal_arglist::=.{

}
formal_arglist::=formal_arg.{

}
formal_arglist::=formal_arglist SYM_COMMA formal_arg. {

}
formal_arg::=type_specifier SYM_IDEN.{


}

/*
 * Statement
 */
%type statement {Statement*}
%type statement_list {GenericVector<Statement>*}
%type for_statement {Statement*}
%type if_statement {Statement*}
%type switch_statement {Statement*}
%type expression_statement {Statement*}

statement_list::=statement.{

}

statement_list::=statement_list statement. {

}

statement ::= return_statement. {

}
statement::=expression_statement.{

}
statement::=for_statement.{

}
statement::=switch_statement.{

}
statement::=if_statement.{

}
return_statement::= SYM_KW_RETURN maybe_expression SYM_SEMI. {

}

expression_statement::=maybe_expression SYM_SEMI. {

}

for_statement::=SYM_KW_FOR SYM_LP maybe_expression SYM_SEMI maybe_expression SYM_SEMI maybe_expression SYM_RP statement. {

}

switch_statement::=SYM_KW_SWITCH SYM_LP expression SYM_RP SYM_LB case_clause_list default_clause SYM_RB.{

}

case_clause_list::=case_clause.{}

case_clause_list::=case_clause case_clause_list.{

}
case_clause::=SYM_KW_CASE constant SYM_COLON statement.{

}
default_clause::=SYM_KW_DEFAULT SYM_COLON statement.{

}
constant::=SYM_INTEGER.{

}
constant::=SYM_CHAR.{

}
/* The precedence of the shorter form is determined by 
 * the first terminal of that rule
 */
%nonassoc SYM_KW_IF.
/* The precedence of the token to shift is itself
 */
%nonassoc SYM_KW_ELSE.

if_statement::=SYM_KW_IF SYM_LP expression SYM_RP statement SYM_KW_ELSE statement.{

}

if_statement::=SYM_KW_IF SYM_LP expression SYM_RP statement.{

}

%type expression {ExpressionStatement*}
%type actual_arglist {GenericVector<ExpressionStatement>*}

expression::=SYM_IDEN . { }
          expression::=SYM_CHAR.{}
expression::=SYM_INTEGER.{}
          expression::=SYM_STRING.{}

expression::=SYM_LP expression SYM_RP. {

}

expression::=expression SYM_ADD expression. { }
          expression::=expression SYM_MINUS expression. { }
expression::=expression SYM_GE expression. { }
          expression::=expression SYM_DIV expression. { }
expression::=expression SYM_MUL expression. { }
          expression::=expression SYM_DEQ expression. { }
expression::=expression SYM_EQ expression. { }
          expression::=expression SYM_LE expression. { }
expression::=expression SYM_LT expression. { }
          expression::=expression SYM_NE expression. { }

expression::=SYM_IDEN SYM_LP actual_arglist SYM_RP. {}

actual_arglist::=.{}

actual_arglist::=expression.{}

actual_arglist::=actual_arglist SYM_COMMA expression.{}


expression::=SYM_MINUS expression. [SYM_RIGHT_UNARY] {}
          expression::=SYM_ADD expression. [SYM_RIGHT_UNARY] {}

%nonassoc SYM_EQ.
%left SYM_DEQ SYM_LT SYM_LE SYM_GE SYM_GT SYM_NE.
%left SYM_ADD SYM_MINUS.
%left SYM_DIV SYM_MUL.
/* This is for unary operator */
%right SYM_RIGHT_UNARY .


