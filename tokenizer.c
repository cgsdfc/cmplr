/* tokenizer.c */
#include "tknzr_table.h"
#include "tknzr_error.h"
#include "tknzr_state.h"
int TK_SLASH;


void init_identifier(void)
{
  int word=alloc_char_class("\\w");
  int Word=alloc_char_class("\\W");
  int init=alloc_state(true);
  int loop=alloc_state(true);
  int fini=alloc_state(false);

  config_action();
    config_from(0);
      config_condition(word);
        add_to(init);
  config_action();
    config_from(init);
      config_condition(Word);
        add_to(init);
  config_action();
    config_usrd(true);
      config_condition(Word);
        add_to(fini);
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
}
      
void init_single_comment(void)
{
  int slash=alloc_char_class("\\\\");
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
}
int get_next_token (token *tk, char_buffer *buf)
{


}

