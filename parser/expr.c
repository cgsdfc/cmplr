#include "grammar.h"

int expr, assign_expr,
    cond_expr, const_expr,
    lor_expr, land_expr,
    bor_expr, bxor_expr, 
    band_expr, equal_expr,
    cmp_expr, shift_expr,
    add_expr, mul_expr,
    cast_expr, unary_expr,
    postfix_expr, primary_expr,
    arglist, unary_oper;

int mul_assign, div_assign,
    mod_assign, add_assign,
    sub_assign, left_shift_assign,
    right_shift_assign, and_assign,
    xor_assign, or_assign;

int identifier, int_const,
    float_const, char_const,
    string_const, 
    left_pare, right_pare,
    left_bck, right_bck,
    star, ampersand,
    minus, minus_minus,
    plus, plus_plus,


int add_oper, sub_oper,
    mul_oper, mod_oper,
    and_oper, or_oper,
    div_oper;

void def_primary(void)
{
  def_nonterm(primary_expr);
    def_one_of(0,
        identifier,
        int_const,
        float_const,
        string_const,
        char_const,
        RULE_END);
    def_rule(0,
        left_pare,
        expr,
        right_pare,
        RULE_END);
}

void def_postfix(void)
{
  def_nonterm(postfix_expr);
    def_rule(0,primary_expr,RULE_END);
    def_rule(0,postfix_expr,left_bck,expr,right_bck,RULE_END);
    def_opt(0,postfix_expr,left_pare,arglist, RULE_OPT,right_pare,RULE_END);
    def_rule(0,postfix_expr,dot,identifier,RULE_END);
    def_rule(0,postfix_expr,arrow,identifier,RULE_END);
    def_rule(0,postfix_expr,plus_plus,RULE_END);
    def_rule(0,postfix_expr,minus_minus,RULE_END);
}

void def_unary(void)
{
  def_nonterm(unary_expr);
    def_rule(0,plus_plus,unary_expr,RULE_END);
    def_rule(0,minus_minus,unary_expr,RULE_END);
    def_rule(0,unary_oper,cast_expr, RULE_END);
    def_rule(0,sizeof_oper,unary_expr, RULE_END);
    def_rule(0,sizeof_oper,left_pare,type_name,right_pare,RULE_END);
  def_nonterm(unary_oper);
    def_oneof(0,
        ampersand,
        star,
        plus,
        minus,
        tilde,
        exclaim,
        RULE_END);

}

void def_cast(void)
{
  def_nonterm(cast_expr);
    def_rule(0,unary_expr,RULE_END);
    def_rule(0,left_pare, type_name, right_pare, cast_expr, RULE_END);
}

void def_mul(void)
{
  def_nonterm(mul_expr);
    def_rule(0,cast_expr, RULE_END);
    def_rule(0,mul_expr,mul,cast_expr,RULE_END);
    def_rule(0,mul_expr,div,cast_expr,RULE_END);
    def_rule(0,mul_expr,mod,cast_expr,RULE_END);
}

void def_add(void)
{
  def_nonterm(add_expr);
    def_rule(0,mul_expr, RULE_END);
    def_rule(0,add_expr,add,mul_expr,RULE_END);
    def_rule(0,add_expr,sub,mul_expr,RULE_END);
}
    
void def_shift(void)
{
  def_nonterm(shift_expr);
    def_rule(0,add_expr, RULE_END);
    def_rule(0,shift_expr,left_shift,add_expr,RULE_END);
    def_rule(0,shift_expr,right_shift,add_expr,RULE_END);
}

void def_cmp (void)
{
  def_nonterm(cmp_expr);
    def_rule(0,shift_expr, RULE_END);
    def_rule(0,cmp_expr,cmp_oper,shift_expr);
  def_nonterm(cmp_oper);
    def_oneof(0, less, greater, less_equal, greater_equal, RULE_END);

}

void def_equal(void)
{
  def_nonterm(equal_expr);
    def_rule(0,cmp_expr, RULE_END);
    def_rule(0,equal_expr,equal,cmp_expr,RULE_END);
    def_rule(0, equal_expr,not_equal, cmp_expr,RULE_END);
}

void def_bitwise_and(void)
{
  def_nonterm(band_expr);
    def_rule(0, equal_expr, RULE_END);
    def_rule(0, band_expr, bitwise_and, equal_expr);
}

void def_bitwise_xor(void)
{
  def_nonterm(bxor_expr);
    def_rule(0, band_expr, RULE_END);
    def_rule(0, bxor_expr, bitwise_xor, band_expr);
}

void def_bitwise_or(void)
{
  def_nonterm(bor_expr);
    def_rule(0, bxor_expr, RULE_END);
    def_rule(0, bor_expr, bitwise_or, bxor_expr);
}

void def_logic_and(void)
{
  def_nonterm(land_expr); 
    def_rule(0,bor_expr,RULE_END);
    def_rule(0,land_expr,and_oper,bor_expr,RULE_END);
}

void def_logic_or(void)
{
  def_nonterm(lor_expr);
    def_isalsoa(land_expr);
    def_binary(lor_expr, or_oper, land_expr);
}




} 
static 
void init_terms(void)
{
  mul_assign = alloc_terminal("*=");
  div_assign = alloc_terminal("/=");
  mod_assign = alloc_terminal("%=");
  add_assign = alloc_terminal("&=");
  sub_assign = alloc_terminal("-=");
  left_shift_assign = alloc_terminal(">>=");
  right_shift_assign = alloc_terminal("<<=");
  and_assign = alloc_terminal("&=");
  xor_assign = alloc_terminal("^=");
  or_assign = alloc_terminal("|=");

}

static 
void init_nonterms(void)
{



}


void def_expr(void)
{
  expression = alloc_nonterm();
  assignment_expression = alloc_nonterm();

  def_nonterm(expression);
  def_rule(0,assignment_expression, RULE_END);
  def_rule(0,expression, comma, assignment_expression, RULE_END);
}

void def_assign(void)
{
  condition_expression = alloc_nonterm();
  unary_expression = alloc_nonterm();
  assign_oper = alloc_nonterm();

  
  def_nonterm(assignment_expression);
    def_rule(0, condition_expression, RULE_END);
    def_rule(0, unary_expression, 
        assign_oper,
        assignment_expression,
        RULE_END);

  def_nonterm(assign_oper);
    def_one_of(0, assign,
        mul_assign,
        div_assign,
        mod_assign,
        add_assign,
        sub_assign,
        left_shift_assign,
        right_shift_assign,
        and_assign,
        xor_assign,
        or_assign,
        RULE_END);


}
    

