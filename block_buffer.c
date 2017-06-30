#include "block_buffer.h"
#define BLOCK_BUFFER_MAX_N 10
static block_buffer all_buffer[BLOCK_BUFFER_MAX_N];
static int all_buffer_count;
static
int init_block_buffer(block_buffer *buf, 
    int blksz,
    int init_nblk,
    int elesz );

block_buffer *alloc_block_buffer(
    int blksz,
    int init_nblk,
    int elesz)
{
  if (all_buffer_count == BLOCK_BUFFER_MAX_N)
  {
    return NULL;
  }

  block_buffer *bb=&all_buffer[all_buffer_count++];
  if (init_block_buffer(bb,blksz,init_nblk,elesz)<0)
  {
    return NULL;
  }
  return bb;
}

static
int init_block_buffer(block_buffer *buf, 
    int blksz,
    int init_nblk,
    int elesz )
{
  assert(buf);
  assert(blksz>0);
  assert(elesz>0);
  assert(init_nblk>0);

  buf->index=0;
  buf->blksz=blksz;
  buf->elesz=elesz;

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
      buf->cur=mem;
      prv=mem;
      continue;
    }
    prv->next=mem;
    mem->prev=prv;
    prv=mem;
  }
  mem->next=buf->head;
  buf->head->prev=mem;
  return 0;
}

static
bool block_buffer_is_full(block_buffer *buf)
{
  return 
  (buf->index == buf->blksz) && 
  (buf->cur == buf->head->prev);
}

static
int block_buffer_alloc_blk(block_buffer *buf)
{
  assert (buf);
  assert (block_buffer_is_full(buf));
  
  block_mem *bmem=malloc(sizeof(block_mem));
  if (!bmem)
    return -1;

  char *mem=malloc(buf->elesz * sizeof(char) * buf->blksz);
  if(!mem)
    return -1;
  bmem->mem=mem;

  // tail=bmem
  buf->head->prev->next=bmem;
  bmem->prev=buf->head;
  return 0;
}

// reserve a chrunk of mem in the buf for
// elesz 
char *
block_buffer_alloc (block_buffer *buf)
{
  if (block_buffer_is_full(buf)
      && block_buffer_alloc_blk(buf)<0)
  {
    return NULL;
  }

  if (buf->index == buf->blksz)
  {
    buf->index = buf->elesz;
    buf->cur = buf->cur->next;
    return buf->cur->mem;
  }

  char *alloc=buf->cur->mem + buf->index;
  buf->index += buf->elesz;
  return alloc;

}

// dealloc the most recently alloced object
void
block_buffer_dealloc(block_buffer *buf)
{
  if (buf->cur == buf->head
      && buf->index == 0)
  {
    // nothing inside 
    return;
  }

  if (buf->index == 0)
  {
    buf->cur = buf->cur->prev;
    buf->index = buf->blksz;
    return;
  }

  buf->index -= buf->elesz;
}

