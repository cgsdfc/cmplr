#ifndef RECURSIVE_STMT_H
#define RECURSIVE_STMT_H
#include <stdbool.h>

#define STMT_IS_FUNC_DECLARE(FUNC) bool stmt_is_##FUNC(struct pcontext *context)
struct pcontext;
//=====================================================//
// jump_stmt := goto identifier ;
// | return [expr] ;
// | break ;
// | continue ;
// ;
// jump_node = terminal | unary ;
// goto_node = unary (expr) ;
// return_node = terminal | unary (expr) ;
// break_node, continue_node = terminal ;
//
STMT_IS_FUNC_DECLARE(jump);

//=====================================================//
// iterate_stmt := for ( [expr] ; [expr] ; [expr] ) stmt
// | while ( expr ) stmt
// | do stmt while ( expr ) ;
// ;
// iterate_node = while_node | for_node | do_while_node ;
// while_node = binary (expr, stmt_node) ;
// for_node = binary (ternary(expr, expr, expr), stmt_node) ;
// do_while_node = binary (stmt_node, expr) ;
//
STMT_IS_FUNC_DECLARE(iterate);

//=====================================================//
// select_stmt := if ( expr ) stmt [ else stmt ]
// | switch ( expr ) stmt
// ;
// select_node = if_node | switch_node ;
// if_node = unary (ternary(expr, stmt, stmt)) ;
// switch_node = binary (expr, stmt) ;
//
STMT_IS_FUNC_DECLARE(select);

//=====================================================//
// label_stmt := identifier : stmt
// | case constant_expr : stmt
// | default : stmt
// ;
// label_node = default_node | case_node
// default_node = unary (stmt_node) ;
// case_node = binary (terminal | expr, stmt)
//
STMT_IS_FUNC_DECLARE(label);

//=====================================================//
// compound_stmt := { [declare_list] [stmt_list] }
// compound_node = binary (vector, vetor) ;
//
STMT_IS_FUNC_DECLARE(compound);
//=====================================================//
// exprstmt := expr | NULL
// exprstmt_node = expr | NULL
//
STMT_IS_FUNC_DECLARE(exprstmt);
//=====================================================//
// stmt := label_stmt
// | exprstmt
// | compound_stmt
// | select_stmt
// | iterate_stmt
// | jump_stmt
// ;
// stmt_node;
STMT_IS_FUNC_DECLARE(statement);
#endif // RECURSIVE_STMT_H
