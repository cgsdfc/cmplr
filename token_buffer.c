#include "token_buffer.h"
#define BLOCK_BUFFER_MAX_N 10

static block_buffer all_buffer[BLOCK_BUFFER_MAX_N];
static int all_buffer_count;

// TODO: use block_buffer instead
static char_buffer cbuffer;
static token_buffer *tbuffer;

block_buffer *alloc_buffer(void)
{
  assert (all_buffer_count < BLOCK_BUFFER_MAX_N);
  return &all_buffer[all_buffer_count++];
}

int init_block_buffer(block_buffer *buf, 
    char *name ,
    int blksz,
    int init_nblk,
    int elesz )
{
  assert(name);
  assert(buf);
  assert(blksz>0);
  assert(elesz>0);
  assert(init_nblk>0);

  buf->index=0;
  buf->blksz=blksz;
  buf->elesz=elesz;
  buf->name=name;
  buf->count=0;

  block_mem *mem;
  block_mem *prv;

  for (int i=0;i<init_nblk;++i)
  {
    mem=malloc(sizeof(block_mem));
    if(mem==NULL) 
      return -1;
    mem->mem=malloc(sizeof(char) * elesz * blksz);
    if(mem->mem == NULL)
      return -1;

    if (i==0) {
      buf->head=mem;
      buf->cur=mem->mem;
      prv=mem;
      prv->prev=0;
      continue;
    }
    prv->next=mem;
    mem->prev=prv;
    prv=mem;
  }
  mem->next=head;
  head->prev=mem;
  return 0;
}

  
int init_token_buffer(char *file)
{
  if (init_char_buffer_from_file(&cbuffer, file) < 0)
    return -1;

  tbuffer = alloc_buffer();
  if (init_block_buffer(tbuffer,
        "token's buffer",
        N_TKB_BLKSZ,
        N_TKB_INIT_NBLK,
        sizeof(token*))<0)
  {
    return -1;
  }

  return 0;
}

bool block_buffer_is_full(block_buffer *buf)
{
  return 
  (buf->index == buf->blksz) && 
  (buf->cur == buf->head->prev);
}


int block_buffer_alloc_blk(block_buffer *buf)
{
  assert (buf);
  assert (block_buffer_is_full(buf));
  
  char *mem=malloc(buf->elesz * sizeof(char) * buf->blksz);
  if(mem==NULL)
    return -1;

  buf->head->prev->next=mem;
  mem->prev=buf->head;
  return 0;
}


/* fill one block of the buffer */
int fill_token_buffer(token_buffer *buf)
{
  int index=buf->index;
  block_mem *cur=buf->cur;
  int nfill=N_TKB_BLKSZ;
  int r;
  node err;

  for (int i=0;i<nfill;++i)
  {
    if (block_buffer_is_full(buf) && 
        (block_buffer_alloc_blk(buf)!=0))
    {
      return -1;
    }
    switch (get_next_token(&cbuffer, &tk, &err))
    {
      case 0:
        memcpy (buf->cur->mem+buf->index,
            tk, buf->elesz);
        if (buf->index += buf->elesz == buf->blksz)
        {
          buf->index=0;
          buf->cur = buf->cur->next;
        }
        break;

      default:
        // TODO: erro handle, in ergent
        return r;
    }
  }
  return 0;
}


int next_token(token_buffer *buf, token **tk)
{


}

int prev_token(token_buffer *buf, token **tk)
{


}

int peek_token(token_buffer *buf, token **tk)
{


}

int put_token(token_buffer *buf, token **tk)
{



}


int get_token(token_buffer *buf, token **tk)
{



}

