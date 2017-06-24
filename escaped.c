#include "escaped.h"
#include <limits.h>

typedef struct state_table
{
  int nrows;
  int ncols;
  int *count;
  entry_t **diagram;
  entry_t init_st;

} state_table;



  int
init_state_table(state_table *table, int nrows, int ncols, entry_t init_st)
{
  assert(nrows>0);
  assert(ncols>0);
  assert(init_st>=0);
  assert(table);

  table->nrows=nrows;
  table->ncols=ncols;
  table->init_st=init_st;
  if (NULL == (table->diagram=malloc(sizeof(entry_t*) * nrows)))
  {
    perror("malloc");
    return -1;
  }
  table->count=malloc(sizeof(int) * nrows);

  for (int i=0;i<nrows;++i)
  {
    table->diagram[i]=malloc(sizeof(entry_t) * ncols);
    table->count[i]=0;
  }

  return 0;
} 

  void 
st_add_transfer(state_table *table, entry_t from, entry_t to, entry_t flags, entry_t act, entry_t chcl)
{
  assert(0<=from && from < table->nrows);
  assert(0<= to && to < table->ncols);
  entry_t entry = TFE_MAKE_ENTRY(act,chcl,to,flags);
  table->diagram[from][table->count[from]++]=entry;
}


  void
st_add_initial(state_table *table, entry_t state,entry_t chcl)
{
  st_add_transfer(table,table->init_st,state,0,TFE_ACT_INIT,chcl);
}

  void 
st_add_intermedia(state_table *table, entry_t from, entry_t to, entry_t chcl)
{
  st_add_transfer(table,from,to,0,TFE_ACT_APPEND,chcl);
}

  void 
st_add_accepted(state_table *table, entry_t from, entry_t to,  entry_t chcl)
{
  st_add_transfer(table,from,to,TFE_FLAG_ACCEPTED,TFE_ACT_ACCEPT,chcl);
}

  void
st_add_selfloop(state_table *table, entry_t state, entry_t chcl)
{
  st_add_transfer(table,state,state,0,TFE_ACT_APPEND,chcl);
}

/* reversed versions */
  void 
st_add_intermedia_rev (state_table *table, entry_t from, entry_t to, entry_t chcl)
{
  st_add_transfer(table,from,to,TFE_FLAG_REVERSED,TFE_ACT_APPEND,chcl);
}

  void 
st_add_accepted_rev (state_table *table,entry_t from, entry_t to, entry_t chcl)
{
  st_add_transfer(table,from ,to,TFE_FLAG_REVERSED | TFE_FLAG_ACCEPTED,TFE_ACT_ACCEPT,chcl);
}

  void
st_add_selfloop_rev (state_table *table, entry_t state, entry_t chcl)
{
  st_add_transfer(table,state,state,TFE_FLAG_REVERSED,TFE_ACT_APPEND,chcl);
}

state_table escaped_table;

void init_escaped(void)
{
  /* initial */
  init_state_table(&escaped_table,N_ES_ROWS,N_ES_COLS,ES_INIT);
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
  ['\'']='\'',
  ['0']='\0',

};


entry_t st_do_transfer(state_table *table, entry_t state, char cc, entry_t nonf)
{
  assert (state >= 0 && state < table->nrows);
  entry_t *ent;
  entry_t entry;
  char *chcl;
  int len;

  ent=table->diagram[state];
  len=table->count[state];

  for (int i=0;i<len;++i)
  {
    chcl=char_class2string[TFE_CHAR_CLASS(ent[i])];
    bool in_class = strchr(chcl,cc);
    bool is_reversed = TFE_IS_REVERSED(ent[i]);
    if (in_class && !is_reversed
        || !in_class && is_reversed)
    {
      entry=ent[i];
      ent[i]=ent[0];
      ent[0]=entry;
      return entry;
    }
  }

  return nonf;
}

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

/* ch is the first char in the string */
/* if ch is a quote, return ptr to next char */
/* else return ch */
static char *skip_quote_head(char *ch)
{
  if(is_quote(ch[0]))
    return ch+1;
  else
    return ch;
}

/* ch is the char when '\0' should go */
/* if char before ch is a quote, return */
/* the char before ch, else return ch */ 
static char *skip_quote_tail(char *ch)
{
   if(is_quote(*(ch-1)))
    return ch-1;
  else
    return ch;
}

  void
check_skip_quote(void)
{
  char *no_q="no_q";
  assert(*skip_quote_head(no_q)=='n');
  assert(*skip_quote_tail(no_q + strlen(no_q))==0);

  char *h_q="\"h";
  assert(*skip_quote_head(h_q)=='h');
  assert(*skip_quote_tail(h_q + strlen(h_q))==0);

  char *t_q="t_q\"";
  assert(*skip_quote_head(t_q)=='t');
  assert(*skip_quote_tail( t_q + strlen(t_q) ) =='\"');

  char *d_q="\"double quoted\"";
  assert(*skip_quote_head(d_q)=='d');
  assert(*skip_quote_tail((d_q+strlen(d_q))) == '\"' );

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
#if 0
  if (len >= 2){
    if(is_quote(src[len-1])) {
      src[len-1]=0;
    }
    if( is_quote(src[0])) {
      src++;
    }
  }
#endif


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

int des(char *src, char *dst)
{
  /* donot ignore error when checking */
  return eval_string(src,dst);
}

void fini_escaped(void)
{
  memset (&escaped_table, 0, sizeof escaped_table);
}

void check_escaped(void)
{
  puts("check_escaped begin");
  check_skip_quote();
  puts("check_skip_quote passed");

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

