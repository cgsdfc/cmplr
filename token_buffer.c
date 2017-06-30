#include "token_buffer.h"
#include "block_buffer.h"
#include "tokenizer.h"

// there should be one and only one
// perfect token_buffer. it does
// memory alloc of tokens for tokenizer and
// lookahead for parser
static token_buffer tbuffer;
  
int init_token_buffer(char_buffer *cb)
{
  if (!cb) 
    return -1;

  tbuffer.bb=alloc_block_buffer(
        N_TKB_BLKSZ,
        N_TKB_INIT_NBLK,
        sizeof(token));

  if (!tbuffer.bb)
  {
    return -1;
  }
  tbuffer.cb=cb;
  tbuffer.index=0;
  tbuffer.limit=0;
  tbuffer.cur=tbuffer.bb->cur;
  init_tokenizer();
  return 0;
}


/* fill one block of the buffer */
static
int fill_token_buffer(void)
{
  int r;
  int nfill;

  nfill=tbuffer.bb->blksz;
  
  for (int i=0;i<nfill;++i)
  {
    token *tk= (token*) block_buffer_alloc(tbuffer.bb);
    if (!tk)
      return -1;
    memset(tk, 0, sizeof(token));
    switch (get_next_token(tk, tbuffer.cb))
    {
      case 0:
        tbuffer.limit++;
        continue;
      case EOF:
        block_buffer_dealloc(tbuffer.bb);
        return EOF;
    }
  }
  return 0;
}
// tokenizer may not bother the parser
// by throwing error, tokenized error is
// hard to recover and should cause an exit
// (since the tknzr is very tolerant.

// since we fill the buffer 
// one block at a time, the condition
// telling end is index == blksz


int next_token(token **tk)
{
  if (tbuffer.index + 1 == tbuffer.bb->blksz)
  {
      if( fill_token_buffer()==EOF)
        return EOF;
      tbuffer.index=0;
      tbuffer.cur=tbuffer.cur->next;
  }

  char *curtk=tbuffer.cur->mem + (tbuffer.index+1) * sizeof(token);
  *tk= (token*) curtk;
  return 0;

}

int prev_token(token **tk)
{
  if (tbuffer.index==0)
  {
    char *prevtk=tbuffer.cur->prev->mem + (tbuffer.bb->blksz * sizeof(token));
    *tk=(token*)prevtk;
    return 0;
  }
  *tk=(token*) tbuffer.cur->mem + (tbuffer.index-1) * sizeof(token);
  return 0;

}

int peek_token(token **tk)
{
  if (tbuffer.index  == tbuffer.bb->blksz)
  {
      if( fill_token_buffer()==EOF)
        return EOF;
      tbuffer.index=0;
      tbuffer.cur=tbuffer.cur->next;
  }

  char *curtk=tbuffer.cur->mem + (tbuffer.index * sizeof(token));
  *tk=(token*)curtk;
  return 0;

}

int put_token(void)
{
  if (tbuffer.index == 0)
  {
    tbuffer.cur=tbuffer.cur->prev;
    tbuffer.index=tbuffer.bb->blksz;
  }

  tbuffer.index--;
  return 0;

}


int get_token(token **tk)
{
  if (next_token(tk) < 0)
    return -1;

  if (tbuffer.index == tbuffer.bb->blksz)
  {
    tbuffer.cur=tbuffer.cur->next;
    tbuffer.index=0;
  }

  tbuffer.index++;
  return 0;

}

