#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "lexer/block_buffer.h"
#define BLOCK_BUFFER_MAX_N 10
/** note: index points to the **Nth** element,
 * not the **Nth** char!
 */

static block_buffer all_buffer[BLOCK_BUFFER_MAX_N];
static int all_buffer_count;
static
  int init_block_buffer (block_buffer * buf,
			 int blksz, int init_nblk, int elesz);

block_buffer *
alloc_block_buffer (int blksz, int init_nblk, int elesz)
{
  if (all_buffer_count == BLOCK_BUFFER_MAX_N)
    {
      return NULL;
    }

  block_buffer *bb = &all_buffer[all_buffer_count++];
  if (init_block_buffer (bb, blksz, init_nblk, elesz) < 0)
    {
      return NULL;
    }
  return bb;
}

static int
init_block_buffer (block_buffer * buf, int blksz, int init_nblk, int elesz)
{
  assert (buf);
  assert (blksz > 0);
  assert (elesz > 0);
  assert (init_nblk > 0);

  buf->freemem.index = 0;
  buf->blksz = blksz;
  buf->elesz = elesz;

  block_mem *mem;
  block_mem *prv;

  for (int i = 0; i < init_nblk; ++i)
    {
      mem = malloc (sizeof (block_mem));
      if (mem == NULL)
	return -1;
      mem->mem = malloc (sizeof (char) * elesz * blksz);
      if (mem->mem == NULL)
	return -1;

      if (i == 0)
	{
	  buf->head = mem;
	  buf->freemem.blk = buf->head;
	  prv = mem;
	  continue;
	}
      prv->next = mem;
      mem->prev = prv;
      prv = mem;
    }
  mem->next = buf->head;
  buf->head->prev = mem;
  return 0;
}



int
block_buffer_next (block_buffer * buf, block_pos * here, block_pos * next)
{
  if (here->index == buf->blksz - 1)
    {
      next->index = 0;
      next->blk = here->blk->next;
    }
  else
    {
      next->index = here->index + 1;
      next->blk = here->blk;
    }
  return 0;
}

int
block_buffer_prev (block_buffer * buf, block_pos * here, block_pos * next)
{
  if (here->index == 0)
    {
      next->index = buf->blksz - 1;
      next->blk = here->blk->prev;
    }
  else
    {
      next->index = here->index - 1;
      next->blk = here->blk;
    }
  return 0;
}

void *
block_buffer_get_elem (block_buffer * buf, block_pos * where)
{
  if (0 <= where->index && where->index < buf->blksz)
    {
      return where->blk->mem + where->index * buf->elesz;
    }
  return NULL;
}


static bool
block_buffer_is_full (block_buffer * buf)
{
  return
    (buf->freemem.index == buf->blksz) &&
    (buf->freemem.blk == buf->head->prev);
}

static int
block_buffer_alloc_blk (block_buffer * buf)
{
  assert (buf);
  assert (block_buffer_is_full (buf));

  block_mem *bmem = malloc (sizeof (block_mem));
  if (!bmem)
    return -1;

  char *mem = malloc (buf->elesz * sizeof (char) * buf->blksz);
  if (!mem)
    return -1;
  bmem->mem = mem;

  // tail=bmem
  buf->head->prev->next = bmem;
  bmem->prev = buf->head;
  buf->head->prev = bmem;
  bmem->next = buf->head;
  return 0;
}

// reserve a chrunk of mem in the buf for
// elesz 
char *
block_buffer_alloc (block_buffer * buf)
{
  if (block_buffer_is_full (buf) && block_buffer_alloc_blk (buf) < 0)
    {
      return NULL;
    }

  if (buf->freemem.index == buf->blksz)
    {
      buf->freemem.index = 0;
      buf->freemem.blk = buf->freemem.blk->next;
    }

  char *alloc = buf->freemem.blk->mem + (buf->elesz * buf->freemem.index);
  buf->freemem.index++;
  return alloc;

}

// dealloc the most recently alloced object
void
block_buffer_dealloc (block_buffer * buf)
{
  if (buf->freemem.blk == buf->head && buf->freemem.index == 0)
    {
      // nothing inside 
      return;
    }

  if (buf->freemem.index == 0)
    {
      buf->freemem.blk = buf->freemem.blk->prev;
      buf->freemem.index = buf->blksz;
      return;
    }

  buf->freemem.index--;
}

bool
block_pos_equal (block_pos * s, block_pos * t)
{
  return (s->index == t->index && s->blk == t->blk);
}
