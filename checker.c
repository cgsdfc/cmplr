#include "checker.h"
#include "transfer.h"
#include "tknz_table.h"
#include "operator.h"
#include "token.h"
#include "char_class.h"
#include "escaped.h"
#include <limits.h>



/* short hand */
int des(char *src, char *dst)
{
  /* donot ignore error when checking */
  return eval_string(src,dst);
}


void check_escaped(void)
{
  puts("check_escaped begin");

  fini_escaped();
  init_escaped();
  char dst[BUFSIZ];
  char src[BUFSIZ]="a";
  int r=0;

  /* should copy src to dst; */
  r=des(src,dst);
  puts(dst);
  assert(r==0);
  assert (strcmp (dst, src)==0);
  puts("single char passed");

  r=des("", dst);
  assert(r==0);
  assert (strcmp(dst,"")==0);
  puts("empty string passed");

  r=des("3333333333", dst);
  assert(r==0);
  assert (strcmp(dst,"3333333333")==0);
  puts("not-a-escaped string passed");


  /* check each single escaped */
  src[0]='\\';
  src[2]=0;
  for (int i=0;i<strlen(CC_SINGLE_ESCAPE);++i)
  {
    char es=CC_SINGLE_ESCAPE[i];
    src[1]=es;
    r=des(src,dst);
    assert (r==0);
    assert (dst[0] == escaped_tab[es]);
  }
  puts("single char escaped passed");

  /* check oct of diff digits */
  char *octs[]={
    "\1", "\01", "\001", "\112"
  };

  for (int i=1;i<=3;++i) {
    for (char c=0;c<SCHAR_MAX;++c) 
    {
      snprintf(src, BUFSIZ, "\\%.*o",i, c);
      r=des(src,dst);
      assert(r==0);
      assert(c == dst[0]);
    }
    printf("oct of %d dig passed\n", i);
  }
  puts("all oct escaped passed");

  for (int i=1;i<=2;++i) {
    for (char c=0;c<SCHAR_MAX;++c) 
    {
      snprintf(src, BUFSIZ, "\\x%.*x",i, c);
      r=des(src,dst);
      assert(r==0);
      assert(c == dst[0]);
    }
    printf("hex of %d dig passed\n", i);
  }

  puts("all hex escaped passed");

  des("My name is\0YOU CAN NOT SEE", dst);
  assert (strcmp(dst, "My name is")==0);


  fini_escaped();
  puts("check_escaped passed");

}


void check_char_class(void)
{
  puts("check_char_class begin");
  for (int i=CHAR_CLASS_EMPTY;i<_CHAR_CLASS_NULL;++i)
  {
    assert (char_class2string[i]);
  }
  puts("check_char_class passed");
}


void check_append_fixlen(void)
{
  token *tk;
  position pos;
  tk=init_fixlen(&pos,'0');
  int r;

  for (int i=0;i<TOKEN_FIXLEN_MAX_LEN-1;++i)
  {
    r=append_fixlen(tk,'a');
    assert(r==0);
  }
  puts(TOKEN_FIXLEN_STRING(tk));
  r=append_fixlen(tk,'a');
  assert (r!=0);
  puts("check_append_fixlen passed");
}


void check_state2operator(void)
{
  for (int i=TK_INIT;i<MAX_TRANSFER_ENTRIES;++i)
  {
    if (is_oper_type(i))
      assert(state2operator[i]);
  }
}

void check_init_token(void)
{
  token *tk;
  position pos={0};
  position *begin=&pos;
  char tstring[TOKEN_FIXLEN_MAX_LEN -1 ]={'a'};
  char sstring[TOKEN_VARLEN_INIT_LEN ]={'b'};
  char *string;
  int len;

  puts("check_init_token begin");
  for (int i=TFE_BRIEF;i<_TFE_LEN_TYPE_END;++i)
  {
    switch(i) {
      case TFE_FIXLEN:
        tk=init_fixlen(begin,'1');
        len = ((fixlen_token*)tk)->len;
        string = ((fixlen_token*)tk)->string;
        assert (TOKEN_LEN_TYPE(tk) == TFE_FIXLEN);
        assert (len  == 0);
        strncpy (string, tstring, TOKEN_FIXLEN_MAX_LEN);
        puts(string);
        break;

      case TFE_VARLEN:
        tk=init_varlen(begin,'1');
        len = ((varlen_token*)tk)->len;
        string = ((varlen_token*)tk)->string;
        assert (TOKEN_LEN_TYPE(tk) == TFE_VARLEN);
        assert (len  == 0);
        strncpy (string, tstring, TOKEN_VARLEN_INIT_LEN);
        puts(string);
        break;

      case TFE_BRIEF:
        break;
    }
  }
  puts("check_init_token passed");
}

void check_counters(void)
{
  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)
  {
    assert(tknzr_entry_counters[i] == 0);
  }
}


void check_tknzr_table (void) 
{
  puts("check_tknzr_table begin");
  check_char_buffer(); 
  check_operators();
  check_fields();
  check_char_class();

  init_tknzr_table ();
  check_table();
  check_init_len_type();
  clear_tknzr_table();

  check_escaped();
  puts ("check_table passed");

}

void check_fields(void)
{
  entry_t entry;
  entry_t flag, action, state, char_class;
  puts("check fields begin");

  /* check all the action fields */
  for (int i=TFE_ACT_INIT;i<TFE_ACT_SKIP+1;++i)
  {
    entry = TFE_MAKE_ENTRY(i, 0, 0,0); 
    action = TFE_ACTION(entry);
    assert (action == i );
  }
  puts ("check action passed");

  /* check all the char_class fields */
  for (int i=CHAR_CLASS_EMPTY;i<_CHAR_CLASS_NULL;++i)
  {
    entry = TFE_MAKE_ENTRY(0, i,0,0);
    char_class=TFE_CHAR_CLASS (entry);
    assert (char_class== i);
    assert (char_class2string[i]!=NULL);
  }
  puts ("check char_class passed");


  /* check all the state fields */
  for (int i=TK_INIT;i<TK_NULL+1;++i)
  {
    entry = TFE_MAKE_ENTRY(0, 0,i,0);
    state=TFE_STATE(entry);
    assert (state == i);
  }

  puts ("check state passed");

  for (int l=0;l<_TFE_LEN_TYPE_END;++l)
  {
    TFE_SET_LEN_TYPE(entry,l);
    assert (TFE_LEN_TYPE(entry) == l);
  }

  puts("check len_type passed");

  entry = TFE_MAKE_ENTRY(0,0,0,TFE_FLAG_ACCEPTED);
  assert (TFE_IS_ACCEPTED(entry));

  entry = TFE_MAKE_ENTRY(0,0,0,TFE_FLAG_REVERSED);
  assert(TFE_IS_REVERSED(entry));

  puts("check flags passed");

  puts("check all possible combination begin");
  /* use a big loop to test all possible */
  for (int i=TFE_ACT_INIT;i<TFE_ACT_SKIP+1;++i)
  {
    for (int j=CHAR_CLASS_EMPTY;j<_CHAR_CLASS_NULL;++j)
    {
      for (int k=TK_INIT;k<TK_NULL+1;++k)
      {
        for (int l=0;l<_TFE_LEN_TYPE_END;++l)
        {
          entry=TFE_MAKE_ENTRY(i,j,k,0);
          TFE_SET_LEN_TYPE(entry,l);
          assert (j==TFE_CHAR_CLASS(entry));
          assert (i==TFE_ACTION(entry));
          assert (k==TFE_STATE(entry));
          assert (TFE_LEN_TYPE(entry) == l);
        }
      }
    }
  }

  puts ("check_fields passed");

}

/** for each char in `char_class`, check `from` can transfer to `to` */
void check_can_transfer (tokenizer_state from, 
    tokenizer_state to, char_class_enum cc)
{
  char *char_class=char_class2string[cc];
  for (int i=0;i<strlen(char_class);++i) 
  {
    int letter = char_class[i];
    assert (can_transfer(seek_entry(from, to), letter));

  }

}

void check_table (void)
{
  // TODO: check more throughly
  puts("check_table begin");

  /* TK_INIT -> TK_IDENTIFIER_BEGIN */
  check_can_transfer(TK_INIT,TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);

  /* check TK_INIT -> TK_INIT */ 
  check_can_transfer(TK_INIT,TK_INIT,CHAR_CLASS_SPACES);

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_END */
  check_can_transfer(TK_IDENTIFIER_BEGIN,TK_IDENTIFIER_END,CHAR_CLASS_SEPARATOR);

  /* check TK_IDENTIFIER_BEGIN -> TK_IDENTIFIER_BEGIN */
  check_can_transfer(TK_INIT,TK_IDENTIFIER_BEGIN,CHAR_CLASS_IDENTIFIER_BEGIN);

  /* check TK_INIT -> TK_PUNCTUATION_END */
  check_can_transfer(TK_INIT,TK_PUNCTUATION_BEGIN,CHAR_CLASS_PUNCTUATION);

  check_can_transfer(TK_STRING_LITERAL_BEGIN,TK_STRING_LITERAL_ESCAPED,
      CHAR_CLASS_BACKSLASH);

  printf ("check_init_table passed\n");

}

void check_init_len_type(void)
{

  for (int i=0;i<MAX_TRANSFER_ENTRIES;++i)
  {
    for (int j=0;j<tknzr_entry_counters[i];++j)
    {
      entry_t entry = tknzr_table[i][j];
      token_len_type len_type = TFE_LEN_TYPE(entry);
      node to = TFE_STATE(entry);

      if (state_is_brief(to))
        assert(len_type == TFE_BRIEF);

      else if (state_is_fixlen(to)) {
        assert(len_type == TFE_FIXLEN);
      }


      else if (state_is_varlen(to))
        assert(len_type == TFE_VARLEN);

    }
  }
  puts("check_init_len_type passed");
}  
void check_peek_line (char_buffer *buffer)
{
}

void check_char_buffer (void)
{
  char_buffer buf;
  init_char_buffer_from_string(&buf,"?\n\n");
  char ch;

  ch=get_char(&buf);
  assert(ch=='?');
  put_char(&buf);
  assert(peek_char(&buf)=='?');

  ch=get_char(&buf);
  assert(ch == '?');
  ch=get_char(&buf);
  assert(ch == '\n');
  put_char(&buf);
  assert(peek_char(&buf) == '\n');


}

void check_operators(void)
{
  puts("check_operators begin");
  int op_count=0;
  int rej_count=0;
  op_struct *ops=NULL;
  char_class_enum *op;
  char_class_enum *rejc;
  op_struct null_ops={0};

  for (ops=operators; memcmp(ops, &null_ops, sizeof null_ops)!=0;ops++)
  {
    /* init func is not null */
    assert (ops->init);

    /* count reject */
    for (rejc=ops->reject;*rejc;++rejc);
    /* do nothing */

    rej_count=rejc-ops->reject;
    assert (rej_count == ops->op_count);


    /* count op */
    for (op=ops->op;*op;++op); 
    /* do nothing */

    op_count=op - ops->op;
    assert (op_count == ops->op_count);

    /* count states */
    for (int i=0;i<op_count;++i)
    {
      assert (ops->states[i]!=NULL);
      for (int j=0;j<ops->op_states;++j)
      {
        assert (ops->states[i][j]!=0);
      }

    }
  }

  assert (ops-operators == N_OPERATOR_KINDS);
  printf("check_operators passed\n");
}


