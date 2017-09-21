#include "token_buffer.h"
#include "block_buffer.h"
#include "tokenizer.h"
#define N_TOKEN_FILL  1

static int fill_token_buffer (token_buffer * buf, int nfill);

// there should be one and only one
// perfect token_buffer. it does
// memory alloc of tokens for tokenizer and
// lookahead for parser

// TODO make the char_buffer part of token_buffer
int
init_token_buffer (token_buffer * buf, char_buffer * cb)
{
  if (!cb)
    return -1;
  block_buffer *bb;

  bb = alloc_block_buffer (N_TKB_BLKSZ, N_TKB_INIT_NBLK, sizeof (token));

  if (!bb)
    {
      return -1;
    }
  buf->cb = cb;
  // keep sync with freemem
  memcpy (&buf->curtk, &bb->freemem, sizeof (block_pos));
  memcpy (&buf->limit, &buf->curtk, sizeof (block_pos));
  buf->bb = bb;
  init_tokenizer ();
  if (fill_token_buffer (buf, N_TOKEN_FILL) < 0)
    return -1;

  return 0;
}


/* fill one block of the buffer */
static int
fill_token_buffer (token_buffer * buf, int nfill)
{
  int r;
  block_pos next;

  for (int i = 0; i < nfill; ++i)
    {
      token *tk = (token *) block_buffer_alloc (buf->bb);
      if (!tk)
	return -1;
      memset (tk, 0, sizeof (token));
      switch (get_next_token (buf->cb, tk))
	{
	case 1:
	  block_buffer_dealloc (buf->bb);
	  r = tknzr_error_handle (buf->cb);
	  if (r == 0)
	    continue;
	  exit (r);
	  // TODO free up all memory 

	case 0:
	  // increaments the pointer
	  block_buffer_next (buf->bb, &buf->limit, &next);
	  memcpy (&buf->limit, &next, sizeof (block_pos));
	  continue;
	case EOF:
	  block_buffer_dealloc (buf->bb);
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

// the next_token, prev_token and peek_token
// **should** not modify the `curtk` pointer


int
next_token (token_buffer * buf, token ** tk)
{
  // for simplicity, each time alloc **one** block
  // so if one block is consumed, we hit the end
  block_pos next;
  block_buffer_next (buf->bb, &buf->curtk, &next);
  if (block_pos_equal (&next, &buf->limit))
    {
      if (fill_token_buffer (buf, N_TOKEN_FILL) == EOF)
	return EOF;
    }

  *tk = (token *) block_buffer_get_elem (buf->bb, &next);
  return 0;

}

int
prev_token (token_buffer * buf, token ** tk)
{
  int index;
  block_mem *blk;
  block_pos prev;
  block_buffer_prev (buf->bb, &buf->curtk, &prev);

  if (prev.blk == buf->bb->head && prev.index == 0)
    {
      return EOF;
    }
  *tk = (token *) block_buffer_get_elem (buf->bb, &prev);
  return 0;

}

// return the token under the index
// if EOF reached, return EOF,
// if error happened , return -1
// success returns 0
int
peek_token (token_buffer * buf, token ** tk)
{
  if (block_pos_equal (&buf->curtk, &buf->limit))
    {
      if (fill_token_buffer (buf, N_TOKEN_FILL) == EOF)
	return EOF;
    }
  *tk = (token *) block_buffer_get_elem (buf->bb, &buf->curtk);
  return 0;

}

// the put_token, get_token must update
// the `curtk`
// note that, **one** call to put_token
// must happen after **one** call to get_token
// so no need to check 
int
put_token (token_buffer * buf)
{
  block_pos prev;
  block_buffer_prev (buf->bb, &buf->curtk, &prev);
  memcpy (&buf->curtk, &prev, sizeof (block_pos));
  return 0;

}


int
get_token (token_buffer * buf, token ** tk)
{
  if (peek_token (buf, tk) == EOF)
    return EOF;

  block_pos next;
  block_buffer_next (buf->bb, &buf->curtk, &next);
  memcpy (&buf->curtk, &next, sizeof (block_pos));
  return 0;

}
