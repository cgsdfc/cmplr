%include {
#include "Symbol.h"
#include "Scanner.h"
}
%type program {Program*}
%type const_decl_list {GenericVector<ConstDecl>*}
%type function {Function*}
%token_type {TokenValue*}
%token_destructor { delete $$; }
%extra_argument { Parser *parser}

program ::= const_decl_list var_decl_list func_decl_list. {
  
}

/* 
 * ConstDecl
 */
const_decl ::= SYM_KW_CONST SYM_KW_INT SYM_IDEN SYM_EQ SYM_INTEGER SYM_SEMI . {
 

}

const_decl_list ::= . { 

}
const_decl_list ::= const_decl . {

}
const_decl_list ::= const_decl_list const_decl . {

}

const_decl ::= SYM_KW_CONST const_def_list SYM_SEMI. {

}

const_def_list ::= int_const_def_list. {

}
const_def_list  ::= char_const_def_list. {

}

int_const_def_list ::= SYM_KW_INT iden_int_pair_list . {

}
maybe_signed_int::=SYM_ADD SYM_INTEGER.{


}
maybe_signed_int::=SYM_MINUS SYM_INTEGER.{

}
maybe_signed_int::=SYM_INTEGER.{

}
iden_int_pair::= SYM_IDEN SYM_EQ maybe_signed_int. {

}
iden_int_pair_list::= iden_int_pair. {
                  
}
iden_int_pair_list::= iden_int_pair_list SYM_COMMA iden_int_pair. {

}

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

type_specifier::= SYM_KW_CHAR. {

}
type_specifier::=SYM_KW_VOID. {

}
/*
 * VarDecl
 */
var_decl_list::=.{

}
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
func_decl_list ::= type_specifier SYM_IDEN SYM_LP formal_arglist SYM_RP SYM_LB const_decl_list var_decl_list statement_list SYM_RB.
{
    
}

formal_arglist::=.{

}
formal_arglist::=formal_arg.{

}
formal_arglist::=formal_arglist SYM_COMMA formal_arg. {

}
formal_arg::=type_specifier SYM_IDEN. {


}

/*
 * Statement
 */
statement_list::=.{}

statement_list::=statement.{

}

statement_list::=statement_list statement. {

}

statement ::= SYM_SEMI. {

}
