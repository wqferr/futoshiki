#ifndef _BITARRAY_H_
#define _BITARRAY_H_ 1

#include <stddef.h>
#include <stdbool.h>

typedef struct BitArray BitArray;

BitArray *bitarray_new(size_t);
void bitarray_destroy(BitArray *);

size_t bitarray_getNBits(const BitArray *);

unsigned char bitarray_getByte(const BitArray *, size_t);

bool bitarray_check(const BitArray *, size_t);

void bitarray_set(BitArray *, size_t);
void bitarray_unset(BitArray *, size_t);
void bitarray_toggle(BitArray *, size_t);

void bitarray_setBit(BitArray *, size_t, bool);

#endif /* ifndef _BITARRAY_H_ */
