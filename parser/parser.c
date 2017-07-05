#include "rules.h"

void init_clang(void)
{
  DEF_GRAMMAR();
  int program=DEF_NONTERM("program");
  int tran_unit=DEF_NONTERM( "translation-unit");
  int ext_dclr=DEF_NONTERM( "external-declaration");
  int func_def=DEF_NONTERM("function-definition");
  int dclr=DEF_NONTERM("declaration");
  int dclr_spfr=DEF_NONTERM("declaration-specifier");
  int dcltor=DEF_NONTERM("declarator");
  int dclist=DEF_NONTERM("declaration-list");
  int block=DEF_NONTERM( "composite-statement");
  int init_dcltor_list=DEF_NONTERM( "init-declarator-list");
  int stcl_spfr=DEF_NONTERM( "storage-class-specifier");
  int type_spfr=DEF_NONTERM("type-specifier");
  int type_qlfr=DEF_NONTERM("type-qualifier");
  int struct_union_spfr=DEF_NONTERM("struct-or-union-specifier");
  int enum_spfr=DEF_NONTERM("enum-specifier");
  int struct_or_union=DEF_NONTERM("struct-or-union");
  int struct_dclist=DEF_NONTERM("struct-declaration-list");
  int struct_dclr=DEF_NONTERM("struct-declaration");
  int init=DEF_NONTERM("initializer");
  int init_dclr=DEF_NONTERM("init-declarator");
  int spfr_qlfr_list=DEF_NONTERM("specifier-qualifier-list");
  int struct_dcltor_list=DEF_NONTERM("struct-declarator-list");
  int struct_dcltor=DEF_NONTERM("struct-declarator");
  int const_expr=DEF_NONTERM("constant-expression");
  int enumtor_list=DEF_NONTERM("enumerator-list");
  int enumtor=DEF_NONTERM("enumerator");
  int ptr=DEF_NONTERM("pointer");
  int dir_dcltor=DEF_NONTERM("direct-declarator");
  int param_type_list=DEF_NONTERM("parameter-type-list");
  int id_list=DEF_NONTERM("identifier-list");
  int type_qlfr_list=DEF_NONTERM("type-qualifier-list");
  int para_list=DEF_NONTERM("parameter-list");
  int para_dclr=DEF_NONTERM("parameter-declarator");
  int abs_dcltor=DEF_NONTERM("abstract-declarator");
  int dir_abs_dcltor=DEF_NONTERM("direct-abstract-declarator");
  int init_list=DEF_NONTERM("initializer-list");
  int type_name=DEF_NONTERM("type-name");

  int assign_expr=DEF_NONTERM("assignment expression");




  int id=DEF_TERMINAL("identifier");
  int eof=DEF_TERMINAL("eof");
  int left_brace=DEF_PUNC("{");
  int right_brace=DEF_PUNC("}");
  int left_bkt=DEF_PUNC("[");
  int right_bkt=DEF_PUNC("]");
  int left_pare=DEF_PUNC("(");
  int right_pare=DEF_PUNC(")");
  int equal=DEF_PUNC("=");
  int _enum=DEF_KEYWORD("enum");
  int star=DEF_PUNC("*");
  int comma=DEF_PUNC(",");
  int dot=DEF_PUNC(".");

  /* def_rule(&gr, program, tran_unit, eof,-1); */
  DEF_RULE(program, tran_unit, eof);
  DEF_ONEMORE(tran_unit, ext_dclr);
  DEF_ONEOF(ext_dclr, func_def, dclr);
  DEF_RULE(func_def,RULE_OPT,dclr_spfr,dcltor,RULE_OPT,dclist,block);


  DEF_RULE(dclr, dclr_spfr, RULE_OPT, init_dcltor_list); 
  DEF_ONEMORE(dclist, dclr);

  DEF_RULE(dclr_spfr, stcl_spfr, RULE_OPT, dclr_spfr);
  DEF_RULE(dclr_spfr, type_spfr, RULE_OPT, dclr_spfr);
  DEF_RULE(dclr_spfr, type_qlfr, RULE_OPT, dclr_spfr);

  DEF_ONEOF(stcl_spfr, 
      DEF_KEYWORD("auto"),
      DEF_KEYWORD("register"),
      DEF_KEYWORD("static"),
      DEF_KEYWORD("extern"),
      DEF_KEYWORD("typedef")
      );

  DEF_ONEOF(type_spfr,
      DEF_KEYWORD("void"),
      DEF_KEYWORD("char"),
      DEF_KEYWORD("short"),
      DEF_KEYWORD("int"),
      DEF_KEYWORD("long"),
      DEF_KEYWORD("float"),
      DEF_KEYWORD("double"),
      DEF_KEYWORD("signed"),
      DEF_KEYWORD("unsigned"),
      struct_union_spfr,
      enum_spfr,
      id);

 DEF_ONEOF(type_qlfr,
    DEF_KEYWORD("const"),
    DEF_KEYWORD("volatile"));
 DEF_ONEOF(struct_or_union,
     DEF_KEYWORD("struct"),
     DEF_KEYWORD("union"));

 DEF_RULE(struct_union_spfr,
     struct_or_union,
     RULE_OPT, id,
     left_brace, struct_dclist, right_brace); 
 DEF_RULE(struct_union_spfr, struct_or_union, id);

 DEF_ONEMORE(struct_dclist,struct_dclr);
 DEF_ONEMORE(init_dcltor_list, init_dclr);

 DEF_RULE(init_dclr, dcltor);
 DEF_RULE(init_dclr,
     dcltor, 
     equal,
     init);

 DEF_RULE(struct_dclr,spfr_qlfr_list,struct_dcltor_list);
 DEF_RULE(spfr_qlfr_list,type_qlfr, RULE_OPT, spfr_qlfr_list);
 DEF_RULE(spfr_qlfr_list,type_spfr, RULE_OPT, spfr_qlfr_list);

 DEF_ONEMORE(struct_dcltor_list, struct_dcltor);

 DEF_RULE(struct_dcltor, dcltor);
 DEF_RULE(struct_dcltor,
     RULE_OPT, dcltor, 
     DEF_PUNC(":"),
     const_expr);

 DEF_RULE(enum_spfr,
     RULE_OPT, id,
     left_brace,
     enumtor_list,
     right_brace);

 DEF_RULE(enum_spfr,_enum, id);
 DEF_ONEMORE(enumtor_list,enumtor);
 DEF_RULE(enumtor, id);
 DEF_RULE(enumtor, id, equal, const_expr);
 
 DEF_RULE(dcltor, RULE_OPT, ptr, dir_dcltor);
 DEF_RULE(dir_dcltor,id);
 DEF_RULE(dir_dcltor,left_pare, dcltor, right_pare);
 DEF_RULE(dir_dcltor,dir_dcltor,left_bkt,RULE_OPT, const_expr, right_bkt);
 DEF_RULE(dir_dcltor,dir_dcltor,left_pare, param_type_list,right_pare);
 DEF_RULE(dir_dcltor, dir_dcltor, left_pare, RULE_OPT,id_list, right_pare);

 DEF_RULE(ptr, star, RULE_OPT,type_qlfr_list, RULE_OPT, ptr);
 DEF_ONEMORE(type_qlfr_list, type_qlfr);
 DEF_RULE(param_type_list, para_list);
 DEF_RULE(param_type_list, para_list, comma, dot,dot,dot);
 // TODO let tknzr recogize ...

 DEF_RULE(para_list, para_dclr);
 DEF_SEPMORE(para_list,comma, para_dclr);

 DEF_RULE(para_dclr, dclr_spfr, dcltor);
 DEF_RULE(para_dclr, dclr_spfr, RULE_OPT, abs_dcltor);
 DEF_RULE(id_list, id);
 DEF_SEPMORE(id_list, comma, id);

 DEF_RULE(init, assign_expr);
 DEF_RULE(init, left_brace, init_list, right_brace);
 DEF_RULE(init, left_brace, init_list,comma, right_brace);

 DEF_SEPMORE(init_list, comma, init);

 DEF_RULE(type_name, spfr_qlfr_list, RULE_OPT, abs_dcltor);
 DEF_RULE(abs_dcltor, ptr);
 DEF_RULE(abs_dcltor, RULE_OPT, ptr, dir_abs_dcltor);

 DEF_RULE(dir_abs_dcltor,left_pare,  abs_dcltor, right_pare);
 DEF_RULE(dir_abs_dcltor, 
     RULE_OPT,
     dir_abs_dcltor,
     left_bkt,
     RULE_OPT, const_expr,
     right_bkt);
 DEF_RULE(dir_abs_dcltor, 
     RULE_OPT,
     dir_abs_dcltor,
     left_pare,
     RULE_OPT, param_type_list,
     right_pare);
 




  SHOW_RULES();

}


int main(int argc, char **argv)
{
  init_clang();


}

