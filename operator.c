#include "dfa.h"
#include "chcl.h"
#include "tokenizer.h"

#define N_AMPERSAND 4
#define N_LESS 2
#define N_EXCLAIM 6
extern int TK_SLASH;
extern int TK_NEGATIVE;

const static char *a[]=
{
 "!", "=","*","%","^","/",
 };

const static char *b[]=
{
 "&","|","+","-",
};

const static char *c[]=
{
 "<",">",
};


const static char *A[]=
{
 "!=", "==","=*","%=","=^","=/",
 };

const static char *B[]=
{
 "&=","=|","=+","=-",
};

const static char *C[]=
{
 "<=",">=",
};

void init_operator(void)
{
  int equal=alloc_char_class("=");
  int accepted=alloc_state(false);
  int begin;
  int middle;
  int op;
  int op_equal;

  for (int i=0;i<N_EXCLAIM;++i)
  {
    begin=alloc_state(true);
    op=alloc_char_class(a[i]);
    if (strcmp(a[i], "/")==0)
    {
      TK_SLASH=begin;
    }

    op_equal=alloc_char_class(A[i]);

    config_from(0);
    config_action(TKA_ALLOC_BUF);
    config_condition(op);
    add_to(begin);
    config_from(begin);
    config_action(TKA_ACC_OPER);  
    config_condition(op);
    add_to(accepted);
    config_condition(equal);
    add_to(accepted);
    config_condition(op_equal);
    config_usrd(true);
    add_to(accepted);
    config_end();
  }
  for (int i=0;i<N_AMPERSAND;++i)
  {
    begin=alloc_state(true);
    op=alloc_char_class(b[i]);
    if (strcmp(b[i], "-")==0)
    {
      TK_NEGATIVE=begin;
    }
    op_equal=alloc_char_class(B[i]);

    config_from(0);
    config_action(TKA_ALLOC_BUF);
    config_condition(op);
    add_to(begin);
    config_from(begin);
    config_action(TKA_ACC_OPER);
    config_condition(equal);
    add_to(accepted);
    config_usrd(true);
    add_to(accepted);
    config_end();
  }


  for (int i=0;i<N_LESS;++i)
  {
    begin=alloc_state(true);
    op=alloc_char_class(c[i]);
    op_equal=alloc_char_class(C[i]);

    config_from(0);
    config_action(TKA_ALLOC_BUF);
    config_condition(op);
    add_to(begin);
    config_to(accepted);
    config_action(TKA_ACC_OPER);
    config_condition(op_equal);
    config_usrd(true);
    add_from(begin);
    add_from(middle);
    config_condition(equal);
    config_usrd(false);
    add_from(begin);
    add_from(middle);
    config_from(begin);
    config_condition(op);
    config_action(TKA_COLLECT_CHAR);
    add_to(middle);
    config_end();
  }

  config_from(TK_NEGATIVE);
    config_condition(alloc_char_class(">"));
      config_action(TKA_ACC_OPER);
        add_to(accepted);


}

