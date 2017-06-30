/* tokenizer.c */
#include "dfa.h"
#include "chcl.h"
#include "tokenizer.h"
#define N_TOKENIZER_ROWS 70

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
      
void init_punctuation(void)
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

void init_skipspace(void)
{
  int sp=alloc_char_class("\\S");
  config_action(0);
    config_from(0);
      config_condition(sp);
        config_to(0);
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

void init_integer_literal(void)
{
  int zero=alloc_char_class("\\Z");
  int dec=alloc_char_class("\\d");
  int Dec=alloc_char_class("\\D");
  int Oct=alloc_char_class("\\O");
  int Hex=alloc_char_class("\\H");
  int suffix=alloc_char_class("\\I");
  int x=alloc_char_class("\\X");
  int oct_suffix_dot=alloc_char_class("\\O\\I.");
  int hex_suffix_dot=alloc_char_class("\\H\\I.");
  int dec_suffix_dot_e=alloc_char_class("\\D.\\E");
int oct_suffix_dot_x_e=alloc_char_class("\\O\\I.\\X");
  int dec_begin=alloc_state(true);
  int oct_begin=alloc_state(true);
  int hex_begin=alloc_state(true);
  int int_suffix=alloc_state(true);
  int hex_prefix=alloc_state(true);
  int fini=alloc_state(false);


  config_action(TKA_ALLOC_BUF);
    config_from(0);
      config_condition(zero);
        add_to(TK_INT_ZERO);
      config_condition(dec);
        add_to(dec_begin);
  config_action(TKA_COLLECT_CHAR);
    config_from(TK_INT_ZERO);
      config_condition(Oct);
        add_to(oct_begin);
      config_condition(x);
        add_to(hex_prefix);
    config_from(hex_prefix);
      config_condition(Hex);
        add_to(hex_begin);
    config_from(dec_begin);
      config_condition(Dec);
        add_to(dec_begin);
    config_from(oct_begin);
      config_condition(Oct);
        add_to(oct_begin);
    config_from(hex_begin);
      config_condition(Hex);
        add_to(hex_begin);
    config_to(int_suffix);
      config_condition(suffix);
        add_from(dec_begin);
        add_from(oct_begin);
        add_from(hex_begin);
        add_from(TK_INT_ZERO);
  config_action(TKA_ACC_INT);
    config_to(fini);
      config_usrd(true);
        config_condition(suffix);
          add_from(int_suffix);
        config_condition(dec_suffix_dot_e);
          add_from(dec_begin);
        config_condition(oct_suffix_dot);
          add_from(oct_begin);
        config_condition(hex_suffix_dot);
          add_from(hex_begin);
        config_condition(oct_suffix_dot_x_e);
          add_from(TK_INT_ZERO);
  config_end();
}

int init_tokenizer(void)
{
  table=alloc_dfa(N_TOKENIZER_ROWS, cond_char_class);
  init_punctuation();
  init_identifier();
  init_string_literal();
  init_muli_comment();
  init_single_comment();
  init_float_literal();
  init_integer_literal();
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
            alloc_buffer(tk,&buf->pos);
            collect_char (tk,ch);
            break;
          case TKA_COLLECT_CHAR:
            collect_char (tk,ch);
            break;
          case TKA_ACC_CHAR:
            return accept_char(tk,ch);
          case TKA_ACC_IDFR:
            return  accept_identifier(tk,ch);
          case TKA_ACC_STRING:
            return accept_string(tk,ch);
          case TKA_ACC_PUNC:
            return accept_punctuation (tk,ch);
          case TKA_ACC_FLOAT:
            return accept_float(tk,ch);
          case TKA_ACC_INT:
            return accept_integer(tk,ch);
          case TKA_ACC_OPER:
            return accept_operator(tk,ch);
          default:
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


// TODO install handlers, especailly for 
// the init(0) state -- stray char.
// TODO error lv tab, assign each errno
// a lv, changeable.
// TODO the token_buffer catch the error 
// by get_next_token, it hands it to handler
// function, which should output errmsg and tell
// if should go on or die.


