#include "escaped.h"
#include <limits.h>


state_table escaped_table;

void init_escaped(void)
{
  /* initial */
  init_state_table(&escaped_table,"escaped's table",
      N_ES_ROWS,N_ES_COLS,ES_INIT,0);
  /* use char_is_in_class default */
  st_add_initial(&escaped_table,ES_BEGIN, CHAR_CLASS_BACKSLASH);

  /* intermedia */
  st_add_intermedia(&escaped_table,ES_BEGIN,ES_ZERO, CHAR_CLASS_ZERO);
  st_add_intermedia(&escaped_table,ES_ZERO,ES_OCT_END,CHAR_CLASS_OCT_BEGIN);
  st_add_intermedia(&escaped_table,ES_BEGIN,ES_HEX_BEGIN,CHAR_CLASS_X);
  st_add_intermedia(&escaped_table,ES_HEX_BEGIN,ES_HEX_END,CHAR_CLASS_HEX_BEGIN);
  st_add_intermedia(&escaped_table,ES_ZERO,ES_OCT_BEGIN,CHAR_CLASS_OCT_BEGIN);
  st_add_intermedia(&escaped_table,ES_OCT_BEGIN,ES_OCT_END,CHAR_CLASS_OCT_BEGIN);
  st_add_intermedia(&escaped_table, ES_BEGIN, ES_OCT_BEGIN,CHAR_CLASS_OCT_BEGIN_NOT_ZERO);

  /* accepted-rev */
  st_add_accepted_rev(&escaped_table,ES_END,ES_ZERO,CHAR_CLASS_OCT_BEGIN);
  st_add_accepted_rev(&escaped_table, ES_END,ES_HEX_BEGIN,CHAR_CLASS_HEX_BEGIN);
  st_add_accepted_rev(&escaped_table, ES_END,ES_OCT_BEGIN, CHAR_CLASS_OCT_BEGIN);

  /* accepted */
  st_add_accepted(&escaped_table,ES_HEX_END,ES_END, CHAR_CLASS_HEX_BEGIN);
  st_add_accepted(&escaped_table,ES_OCT_END,ES_END, CHAR_CLASS_OCT_BEGIN);
  st_add_accepted(&escaped_table,ES_BEGIN,ES_END, CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO);

  /* selfloop */
  st_add_selfloop_rev(&escaped_table,ES_INIT,CHAR_CLASS_BACKSLASH);

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
    ent=st_do_transfer(&escaped_table,nsa,*cc, ES_NULL);
    nsa=TFE_STATE(ent);

    switch (nsa) {
      case ES_NULL:
        r=E_BAD_ESCAPED;
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
          r=E_BAD_ESCAPED;
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
    case ES_SKIP:
    case ES_END:
      *dst=0;
      return r;

    case ES_OCT_BEGIN: case ES_OCT_END: case ES_ZERO:
    case ES_HEX_BEGIN: case ES_HEX_END:
      *pesbuf=0;
      value=eval_escaped(esbuf, es_kind);
      if (value < 0)
        r=E_BAD_ESCAPED;

      *dst++=value;
      *dst=0;
      return r;
  }


}


void fini_escaped(void)
{
  memset (&escaped_table, 0, sizeof escaped_table);
}


