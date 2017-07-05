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




  int id=DEF_TERMINAL("identifier");
  int eof=DEF_TERMINAL("eof");
  int left_brace=DEF_PUNC("{");
  int right_brace=DEF_PUNC("}");
  int equal=DEF_PUNC("=");
  int _enum=DEF_KEYWORD("enum");

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
 

  SHOW_RULES();

}


int main(int argc, char **argv)
{
  init_clang();


}

