#include "escaped.h"

const static transfer_entry es_table [][]=
{
  [ES_BEGIN]=
  {
    TFE_MAKE_ENTRY(0,),
  }
};

static
void init_table(void)
{
  add_initial_r(es_table,ES_BEGIN,CHAR_CLASS_BACKSLASH);
  add_selfloop_r(
int do_escaped(char *src, char *dst, int len)
{
  assert (src);
  assert (dst);
  assert (dst > src+len || src > dst+len);
  assert (len>=0);

  char *ps;
  char ch;
  node state,nstate;
  entry_t entry;
  /* if (src[0]=='\''||src[0]=='\"') */
  /*   src++; */
  /* if (src[len-1]=='\''||src[len-1]=='\"') */
  /*   len--; */


  for (ps=src;*ps && len;ps++,len--)
  {
    ch=*ps;
    nstate=do_transfer_r(es_table,state,ch, &entry, ES_NULL);
    if (nstate==ES_NULL)
      return -1;
    state=nstate;
    switch (TFE_ACTION(entry)) {
      case TFE_ACT_ACCEPT:
        switch (nstate) {
          case ES_OCT_END:
            break;
          case ES_HEX_END:
            break;
          case ES_SINGLE_END:
            break;
          default:
            break;
        }
    }
  }

}

const static char escaped_tab[]=
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
  ['0']='\0',

};

void escaped_token(token *tk, node state, char_buffer *buffer)
{
  char _char;
  put_char (buffer);

  _
/** caller should make sure the
 * transfer: whatever -> ES_BEGIN
 * has been made
 */
void add_escape_all (node *state, char_class_enum quote)
{
  add_escape_hex_literal(state,quote);
  add_escape_oct_literal(state,quote);
  add_escape_single(state,quote);
}

void add_escape_hex_literal(node *state, char_class_enum quote)
{

  /* => '\x */
  add_intermedia(state[ES_BEGIN],state[ES_HEX_BEGIN],
     CHAR_CLASS_X);

  /* => '\xf */
  add_intermedia(state[ES_HEX_BEGIN],state[ES_HEX_END],
      CHAR_CLASS_HEX_BEGIN);

  /* => '\xf' */
  add_accepted(state [ES_HEX_END], state [ES_END],
      quote);

  /* => '\xff */
  add_intermedia(state [ES_HEX_END],state [ES_PART],
      CHAR_CLASS_HEX_BEGIN);
}

void add_escape_oct_literal(node *state, char_class_enum quote)
{
  /* => '\0 */
  add_intermedia(state[ES_ZERO],state[ES_OCT_END],
      CHAR_CLASS_OCT_BEGIN);

  /* => '\7 */
  add_intermedia(state[ES_BEGIN],state[ES_OCT_BEGIN],
      CHAR_CLASS_OCT_BEGIN_NOT_ZERO);

  /* => '\7' */
  add_accepted(state[ES_OCT_BEGIN],state [ES_END],
      quote);


  /* => '\71 */
  add_intermedia(state[ES_OCT_BEGIN],state [ES_OCT_END],
      CHAR_CLASS_OCT_BEGIN);

  /* => '\713 */
  add_intermedia(state[ES_OCT_END],state [ES_PART],
      CHAR_CLASS_OCT_BEGIN);

  /* => '\71' */
  add_accepted(state[ES_OCT_END],state [ES_END],
      quote);

}

void add_escape_single(node *state, char_class_enum quote)
{
  /* => '\a (a!=0) */
  add_intermedia(state[ES_BEGIN], state[ES_PART],
      CHAR_CLASS_SINGLE_ESCAPE_NON_ZERO);

  /* => '\0 */
  add_intermedia(state[ES_BEGIN], state[ES_ZERO],CHAR_CLASS_ZERO);

  /* => '\0' */
  add_accepted(state[ES_ZERO],state [ES_END],quote);

}
