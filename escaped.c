#include "escaped.h"
#include <limits.h>

typedef enum escaped_cc
{
  ECC_ZERO,
  ECC_OCT_DIGITS,
  ECC_HEX_DIGITS,
  ECC_OCT_DIGITS_NON_ZERO,
  ECC_SINGLE_ESCAPED_NON_ZERO,
  ECC_BACKSLASH,
  ECC_X,
} escaped_cc;

const static char* cc_tab[]=
{

  [ECC_ZERO ]="0",
  [ECC_OCT_DIGITS ]="01234567",
  [ECC_HEX_DIGITS ]="0123456789abcdefABCDEF",
  [ECC_OCT_DIGITS_NON_ZERO ]="1234567",
  [ECC_SINGLE_ESCAPED_NON_ZERO ]="abfnrtv\\\'\"?",
  [ECC_BACKSLASH ]="\\",
  [ECC_X]="x",

};

static int 
ecc_char_cond(entry_t cond, entry_t ch)
{
  return strchr(cc_tab[cond],ch)!=NULL;
}

state_table *escaped_table;

int init_escaped(void)
{
  escaped_table = alloc_table();
  assert (escaped_table);
  int r=0;

  /* initial */
  r = init_state_table(escaped_table,
      "escaped's table",
      N_ES_ROWS,
      N_ES_COLS,
      ES_NULL,
      ecc_char_cond);
  if (r<0) {
    return -1;
  }

  /* use char_is_in_class default */
  add_initial(ES_BEGIN, ECC_BACKSLASH);

  /* intermedia */
  add_intermedia(ES_BEGIN,ES_ZERO, ECC_ZERO);
  add_intermedia(ES_ZERO,ES_OCT_END,ECC_OCT_DIGITS);
  add_intermedia(ES_BEGIN,ES_HEX_BEGIN,ECC_X);
  add_intermedia(ES_HEX_BEGIN,ES_HEX_END,ECC_HEX_DIGITS);
  add_intermedia(ES_ZERO,ES_OCT_BEGIN,ECC_OCT_DIGITS);
  add_intermedia(ES_OCT_BEGIN,ES_OCT_END,ECC_OCT_DIGITS);
  add_intermedia( ES_BEGIN, ES_OCT_BEGIN,ECC_OCT_DIGITS_NON_ZERO);

  /* accepted-rev */
  add_accepted_rev(ES_END,ES_ZERO,ECC_OCT_DIGITS);
  add_accepted_rev( ES_END,ES_HEX_BEGIN,ECC_HEX_DIGITS);
  add_accepted_rev( ES_END,ES_OCT_BEGIN, ECC_OCT_DIGITS);

  /* accepted */
  add_accepted(ES_HEX_END,ES_END, ECC_HEX_DIGITS);
  add_accepted(ES_OCT_END,ES_END, ECC_OCT_DIGITS);
  add_accepted(ES_BEGIN,ES_END, ECC_SINGLE_ESCAPED_NON_ZERO);

  /* selfloop */
  add_selfloop_rev(ES_INIT,ECC_BACKSLASH);

}

const char escaped_tab[]=
{
  ['a']='\a',
  ['b']='\b',
  ['f']='\f',
  ['n']='\n',
  ['r']='\r',
  ['t']='\t',
  ['v']='\v',
  ['\\']='\\',
  ['\'']= '\'',
  ['\"']='\"',
  ['\?']='\?',
  ['\'']='\'',
  ['0']='\0',

};

// the errors eval_escaped may set
// are who knows
char eval_escaped(char *buf, entry_t kind)
{
  long val;
  switch (kind) {
    case ES_OCT_BEGIN:
      return strtol(buf,0,8);
    case ES_HEX_BEGIN:
      return strtol(buf,0,16);
    case ES_ZERO:
      return 0;
    default:
      val=escaped_tab[buf[0]];
      assert (val!=0);
      return val;
  }
}

  static bool 
is_quote(char ch)
{
  return ch=='\'' || ch == '\"';
}

int eval_string(char *src, char *dst)
{
  assert (src);
  assert (dst);

  escape_state nsa=ES_INIT;
  entry_t ent=0;
  entry_t es_kind=0;
  char *cc;
  char esbuf[10];
  char *pesbuf=esbuf;
  char value;
  int r=0;
  int len = strlen(src);
  // the caller should make sure the quotes are stripped


  for (cc=src;*cc; ++cc) 
  {
    nsa=st_do_transfer(escaped_table,nsa,*cc, &ent);

    switch (nsa) {
      case ES_NULL:
        parser_error_set( PERR_BAD_ESCAPED );
        r=-1;
        *pesbuf=0;
        strcpy(dst,esbuf);
        continue;

      case ES_INIT:
        *dst++ = *cc;
        break;

      case ES_ZERO:
        es_kind=ES_ZERO;
        *pesbuf++= *cc;
        break;

      case ES_OCT_BEGIN: case ES_OCT_END:
        es_kind=ES_OCT_BEGIN;
        *pesbuf++ = *cc;
        break;

        // this 'x' should not go into esbuf
        /* case ES_HEX_BEGIN : */
      case ES_HEX_END:
        es_kind=ES_HEX_BEGIN;
        *pesbuf++=*cc;
        break;

      case ES_END:
        *pesbuf++=*cc;
        *pesbuf=0;
        value=eval_escaped(esbuf, es_kind);
        if (value < 0){
          parser_error_set (PERR_BAD_ESCAPED);
          r=-1;
        }
        *dst++=value;
        nsa=ES_INIT;
        break;
    }
    if (TFE_IS_ACCEPTED(ent) && TFE_IS_REVERSED(ent))
      cc--;
  }

  assert (nsa != ES_BEGIN);

  switch (nsa) {
    case ES_INIT:
    case ES_END:
      *dst=0;
      return r;

    case ES_OCT_BEGIN: case ES_OCT_END: case ES_ZERO:
    case ES_HEX_BEGIN: case ES_HEX_END:
      *pesbuf=0;
      value=eval_escaped(esbuf, es_kind);
      if (value < 0) {
        parser_error_set (PERR_BAD_ESCAPED);
        r=-1;
      }

      *dst++=value;
      *dst=0;
      return r;
  }


}


void fini_escaped(void)
{
  memset (&escaped_table, 0, sizeof escaped_table);
}


