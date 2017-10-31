/* stack_buffer.h */
#ifndef STACK_BUFFER_H
#define STACK_BUFFER_H 1
#include "tknzr/block_buffer.h"

/** stack_buffer impls a linked stack that allows
 * fast insertion and deletion at top.
 * It is used to abstract away the block_buffer, which
 * supports fast peek , prev and next operations but
 * only allocates fix-size objects without ability to
 * dealloc them.
 *
 * TODO: remove the block_buffer_dealloc, it is useless
 *
 * for each stack_buffer, it keeps a block_buffer* to use it
 * alloc objects which then will be managed totally by
 * stack_buffer.
 *
 * this ensures all the objects under a stack_buffer has
 * the same size.
 *
 * and the stack_buffer_entries themselve are of fix size
 * so there is a global block_buffer managing them
 *
 * so now I have a 2 level mm. block_buffer is useful for
 * allocing large chrunck of fix-sized objects and potentially
 * treats them as a stream.
 *
 * stack_buffer is good for alloc and dealloc quickly of var-sized
 * objects.
 */

#define STACK_BUFFER_MAX_LEN 10
#define STACK_BUFFER_BLKSZ 10
#define STACK_BUFFER_INIT_NBLK 1
#define STACK_BUFFER_NFILL 10

typedef union element {
	int int_val;
	void *ptr_val;
} element;

typedef struct stack_buffer_entry {
	struct stack_buffer_entry *next;
	element value;
} stack_buffer_entry;

typedef struct stack_buffer {
	block_buffer *blk_buf;
	stack_buffer_entry *head;
	int nalloc;
	int nused;
} stack_buffer;

stack_buffer *alloc_stack_buffer(int elesz);
int stack_buffer_alloc(stack_buffer * buf, stack_buffer_entry ** ent);
int stack_buffer_dealloc(stack_buffer * buf, stack_buffer_entry * ent);

#endif
