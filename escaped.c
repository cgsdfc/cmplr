#include "escaped.h"
#include <limits.h>


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
      char_is_in_class);
  if (r<0) {
    return -1;
  }

  /* use char_is_in_class default */
  add_initial(ES_BEGIN, CHAR_CLASS_BACKSLASH);

  /* intermedia */
  add_intermedia(ES_BEGIN,ES_ZERO, CHAR_CLASS_ZERO);
  add_intermedia(ES_ZERO,ES_OCT_END,CHAR_CLASS_OCT_BEGIN);
  add_intermedia(ES_BEGIN,ES_HEX_BEGIN,CHAR_CLASS_X);
  add_intermedia(ES_HEX_BEGIN,ES_HEX_END,CHAR_CLASS_HEX_BEGIN);
  add_intermedia(ES_ZERO,ES_OCT_BEGIN,CHAR_CLASS_OCT_BEGIN);
  add_intermedia(ES_OCT_BEGIN,ES_OCT_END,CHAR_CLASS_OCT_BEGIN);
  add_intermedia( ES_BEGIN, ES_OCT_BEGIN,CHAR_CLASS_OCT_BEGIN_NOT_ZERO);

  /* accepted-rev */
  add_accepted_rev(ES_END,ES_ZERO,CHAR_CLASS_OCT_BEGIN);
  add_accepted_rev( ES_END,ES_HEX_BEGIN,CHAR_CLASS_HEX_BEGIN);
  add_accepted_rev( ES_END,ES_OCT_BEGIN, CHAR_CLASS_OCT_BEGIN);

  /* accepted */
  add_accepted(ES_HEX_END,ES_END, CHAR_CLASS_HEX_BEGIN);
  add_accepted(ES_OCT_END,ES_END, CHAR_CLASS_OCT_BEGIN);
  add_accepted(ES_BEGIN,ES_END, CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO);

  /* selfloop */
  add_selfloop_rev(ES_INIT,CHAR_CLASS_BACKSLASH);

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
    nsa=st_do_transfer(escaped_table,nsa,*cc, &ent);

    switch (nsa) {
      case ES_NULL:
        r=PERR_BAD_ESCAPED;
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
      return -1;

    case ES_OCT_BEGIN: case ES_OCT_END: case ES_ZERO:
    case ES_HEX_BEGIN: case ES_HEX_END:
      *pesbuf=0;
      value=eval_escaped(esbuf, es_kind);
      if (value < 0) {
        parser_error_set (PERR_BAD_ESCAPED);
      }

      *dst++=value;
      *dst=0;
      return -1;
  }


}


void fini_escaped(void)
{
  memset (&escaped_table, 0, sizeof escaped_table);
}


