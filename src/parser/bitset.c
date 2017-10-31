#include "bitset.h"
#include <string.h>

bitset *create_bitset(int size)
{
	bitset *self = malloc(sizeof(struct bitset));
	if (self) {
		init_bitset(self, size);
	}
	return self;
}

void init_bitset(bitset * self, int size)
{
	self->m_storage_size = BITSET_SIZE_TO_STORAGE(size) + 1;
	self->m_impl = malloc(self->m_storage_size * BITSET_SIZEOF_IMPL);
}

bool test_bitset(bitset * self, size_t pos)
{
	unsigned *impl = self->m_impl;
	return (impl[pos / BITSET_BITS_OF_IMPL] &
		(1 << pos % BITSET_BITS_OF_IMPL));
}

void set_bitset(bitset * self, size_t pos)
{
	unsigned *impl = self->m_impl;
	impl[pos / BITSET_BITS_OF_IMPL] |=
	    (1 << pos % BITSET_BITS_OF_IMPL);
}

void unset_bitset(bitset * self, size_t pos)
{
	unsigned *impl = self->m_impl;
	impl[pos / BITSET_BITS_OF_IMPL] &=
	    ~(1 << pos % BITSET_BITS_OF_IMPL);
}

void union_bitset_inplace(bitset * self, bitset * other)
{
	for (int i = 0; i < self->m_storage_size; ++i) {
		self->m_impl[i] |= other->m_impl[i];
	}
}

void bitset_copy(bitset * self, bitset * other)
{
	if (self == other)
		return;
	memcpy(self->m_impl, other->m_impl,
	       self->m_storage_size * BITSET_SIZEOF_IMPL);
}
