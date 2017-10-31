#ifndef BITSET_H
#define BITSET_H 1

#include <stdbool.h>
#include <stdlib.h>

#define BITSET_SIZEOF_IMPL (sizeof(unsigned))
#define BITSET_BITS_OF_IMPL ((BITSET_SIZEOF_IMPL) << 3)
#define BITSET_SIZE_TO_STORAGE(size) ((size) / BITSET_SIZEOF_IMPL)
#define BITSET_STORAGE_TO_SIZE(storage) ((storage)*BITSET_SIZEOF_IMPL)

typedef struct bitset {
	unsigned *m_impl;
	size_t m_storage_size;
} bitset;

bitset *create_bitset(int size);
void init_bitset(bitset * self, int size);
bool test_bitset(bitset * self, size_t pos);
void set_bitset(bitset * self, size_t pos);
void unset_bitset(bitset * self, size_t pos);
void union_bitset_inplace(bitset * self, bitset * other);
void bitset_copy(bitset * self, bitset * other);

#endif
