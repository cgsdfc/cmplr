/* tokenizer.c */
#include "dfa.h"
#include "chcl.h"
#include "tokenizer.h"

int TK_SLASH;
int TK_INT_ZERO;
int TK_INT_DEC_BEGIN;
int TK_NEGATIVE;
int TK_DOT;
static dfa_table *table;


void init_identifier(void)
{
  int word=alloc_char_class("\\w");
  int Word=alloc_char_class("\\W");
  int init=alloc_state(true);
  int loop=alloc_state(true);
  int fini=alloc_state(false);

  config_action(TKA_ALLOC_BUF);
    config_from(0);
      config_condition(word);
        add_to(init);
  config_action(TKA_COLLECT_CHAR);
    config_from(init);
      config_condition(Word);
        add_to(init);
  config_action(TKA_ACC_IDFR);
    config_usrd(true);
      config_condition(Word);
        add_to(fini);
  config_end();
}


void init_muli_comment(void)
{
  int star=alloc_char_class("*");
  int multi=alloc_state(true);
  int star_loop=alloc_state(true);
  int char_loop=alloc_state(true);

  config_action(0);
    config_from(TK_SLASH);
      config_condition(star);
        add_to(multi);
    config_from(char_loop);
      config_condition(star);
        config_usrd(true);
          add_to(char_loop);
        config_usrd(false);
          add_to(star);
    config_from(star_loop);
          add_to(0);
        config_usrd(true);
          add_to(char_loop);
  config_end();
}
      
void init_punc(void)
{
  int punc=alloc_char_class("\\p");
  int dot=alloc_char_class(".");
  int dec=alloc_char_class("\\D");
  TK_DOT=alloc_state(true);
  int fini=alloc_state(false);
  
  config_action(TKA_ACC_PUNC);
    config_from(0);
      config_condition(punc);
        add_to(fini);
      config_condition(dot);
        add_to(TK_DOT);
    config_from(TK_DOT);
      config_condition(dec);
        config_usrd(true);
          add_to(fini);
  config_end();
}

void init_single_comment(void)
{
  int slash=alloc_char_class("\\B");
  
  int newline=alloc_char_class("\\N");
  int single=alloc_state(true);


  config_action(0);
    config_from(TK_SLASH);
      config_condition(slash);
        add_to(single);
    config_from(single);
      config_usrd(true);
        config_condition(newline);
          add_to(single);
      config_usrd(false);
          add_to(0);
    config_end();
}

void init_char_literal(void)
{
  int singleq=alloc_char_class("\\q");
  int newline=alloc_char_class("\\N");
  int bs=alloc_char_class("\\B");
  int single_newline_bs=alloc_char_class("\\q\\N\\B");
  int char_part=alloc_state(true);
  int char_begin=alloc_state(true);
  int char_escape=alloc_state(true);
  int char_end=alloc_state(false);

  config_action(TKA_ALLOC_BUF);
    config_from(0);
      config_condition(singleq);
        add_to(char_begin);
  config_action(TKA_COLLECT_CHAR);
    config_from(char_begin);
      config_condition(single_newline_bs);
        add_to(char_part);
      config_condition(bs);
        add_to(char_escape);
    config_from(char_escape);
      config_condition(newline);
        add_to(char_part);
    config_from(char_part);
      config_condition(single_newline_bs);
        add_to(char_part);
  config_action(TKA_ACC_CHAR);
      config_condition(singleq);
        add_to(char_end);
  config_end();

}

void init_float_literal(void)
{
  int Dec=alloc_char_class("\\D");
  int dot=alloc_char_class(".");
  int eE=alloc_char_class("\\E");
  int suffix=alloc_char_class("\\F");
  int sign=alloc_char_class("-+");
  int Dec_suffix=alloc_char_class("\\D\\F");
  int Dec_suffix_e=alloc_char_class("\\D\\F\\E");

  int fraction=alloc_state(true);
  int exponent=alloc_state(true);
  int exp_begin=alloc_state(true);
  int float_sign=alloc_state(true);
  int float_suffix=alloc_state(true);
  int fini=alloc_state(false);

  config_action(TKA_ALLOC_BUF);
    config_to(fraction);
      config_condition(Dec);
        add_from(TK_DOT);
      config_condition(dot);
        add_from(TK_INT_ZERO);
        add_from(TK_INT_DEC_BEGIN);
    config_action(TKA_COLLECT_CHAR);
      config_condition(eE);
        config_to(exp_begin);
          add_from(TK_INT_ZERO);
          add_from(TK_INT_DEC_BEGIN);
          add_from(fraction);
      config_condition(sign);
        config_to(float_sign);
          add_from(exponent);
      config_condition(Dec);
        config_to(exponent);
          add_from(exponent);
          add_from(float_sign);
        config_to(fraction);
          add_from(fraction);
      config_condition(suffix);
        config_to(float_suffix);
          add_from(exponent);
          add_from(fraction);
          add_from(float_suffix);
    config_action(TKA_ACC_FLOAT);
      config_to(fini);
        config_usrd(true);
          config_condition(Dec_suffix);
            add_from(fraction);
            add_from(exponent);
          config_condition(Dec_suffix_e);
            add_from(fraction);
    config_end();
}

void init_string_literal(void)
{
  int str_begin=alloc_state(true);
  int str_escape=alloc_state(true);
  int fini=alloc_state(false);
  int dq_nl_bs=alloc_char_class("\\Q\\N\\B");
  int bs=alloc_char_class("\\B");
  int newline=alloc_char_class("\\N");
  int doubleq=alloc_char_class("\\Q");

  config_action(TKA_ALLOC_BUF);
    config_from(0);
      config_condition(doubleq);
        add_to(str_begin);
  config_from(str_begin);
    config_action(TKA_COLLECT_CHAR);
      config_condition(bs);
        add_to(str_escape);
        config_usrd(true);
      config_condition(dq_nl_bs);
        add_to(str_begin);
    config_action(TKA_ACC_STRING);
      config_condition(doubleq);
        add_to(fini);
  config_from(str_escape);
    config_action(TKA_COLLECT_CHAR);
      add_to(str_begin);
  config_end();

}

int get_next_token (token *tk, char_buffer *buf)
{
  dfa_state *entry=NULL;
  int state=0;
  int r=0;
  int ch;

  while (true)
  {
    ch = get_char(buf);
    r = transfer(table,state,ch,&entry);
    switch (r)
    {
      case 0:
        switch (entry->action)
        {
          case TKA_ALLOC_BUF:
            break;
          case TKA_COLLECT_CHAR:
            break;
        }
        break;
      case 1:
        // exception handler
        tknzr_error_set(entry->action);
      case -1:
        // this error happened in transfer
        // in which transfer_func should 
        // set errno.
        return 1;
    }
  }
}




