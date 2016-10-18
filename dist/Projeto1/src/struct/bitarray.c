#include <stdlib.h>
#include <limits.h>

#include "struct/bitarray.h"

#define FIRST_BIT 0b10000000
#define _bit(n) (FIRST_BIT >> n)
#define _shl(x, n) (x << (CHAR_BIT-1 - n))
#define _check_bit(n, i) (n & _bit(i))

#define _set_bit(n, i) n |= _bit(i)
#define _clear_bit(n, i) n &= ~_bit(i)
#define _toggle_bit(n, i) n ^= _bit(i)
#define _set_bit_to(n, i, x) n = (n & ~_shl(!x, i)) | _shl(x, i)

struct BitArray {
    unsigned char *bytes;
    size_t nBits;
};

BitArray *bitarray_new(size_t nBits) {
    BitArray *a = malloc(sizeof(*a));
    a->bytes = calloc((nBits+CHAR_BIT-1) / CHAR_BIT, sizeof(*a->bytes));
    a->nBits = nBits;
    return a;
}

void bitarray_destroy(BitArray *a) {
    free(a->bytes);
    free(a);
}

size_t bitarray_getNBits(const BitArray *a) {
    return a->nBits;
}

unsigned char bitarray_getByte(const BitArray *a, size_t i) {
    return a->bytes[i];
}

bool bitarray_check(const BitArray *a, size_t i) {
    return _check_bit(
                a->bytes[i/CHAR_BIT],
                i%CHAR_BIT
    );
}

void bitarray_set(BitArray *a, size_t i) {
    _set_bit(
            a->bytes[i/CHAR_BIT],
            i%CHAR_BIT
    );
}

void bitarray_clear(BitArray *a, size_t i) {
    _clear_bit(
            a->bytes[i/CHAR_BIT],
            i%CHAR_BIT
    );
}

void bitarray_toggle(BitArray *a, size_t i) {
    _toggle_bit(
            a->bytes[i/CHAR_BIT],
            i%CHAR_BIT
    );
}

void bitarray_setTo(BitArray *a, size_t i, bool x) {
    _set_bit_to(
            a->bytes[i/CHAR_BIT],
            i%CHAR_BIT, x
    );
}

void bitarray_setAll(BitArray *a) {
    size_t i = ((a->nBits+CHAR_BIT-1) / CHAR_BIT)  -  1;
    while (i > 0) {
        a->bytes[i] = UCHAR_MAX;
        i--;
    }
    a->bytes[0] = UCHAR_MAX;
}

void bitarray_clearAll(BitArray *a) {
    size_t i = ((a->nBits+CHAR_BIT-1) / CHAR_BIT)  -  1;

    while (i > 0) {
        a->bytes[i] = 0;
        i--;
    }
    a->bytes[0] = 0;
}

void bitarray_toggleAll(BitArray *a) {
    size_t i = ((a->nBits+CHAR_BIT-1) / CHAR_BIT)  -  1;

    while (i > 0) {
        a->bytes[i] = ~a->bytes[i];
        i--;
    }
    a->bytes[0] = ~a->bytes[0];
}

void bitarray_setAllTo(BitArray *a, bool x) {
    size_t i = ((a->nBits+CHAR_BIT-1) / CHAR_BIT)  -  1;
    unsigned char b = UCHAR_MAX * (unsigned char) x;

    while (i > 0) {
        a->bytes[i] = b;
        i--;
    }
    a->bytes[0] = b;
}

#undef _set_bit_to
#undef _toggle_bit
#undef _clear_bit
#undef _set_bit
#undef _get_bit
#undef _shl
#undef _bit
#undef FIRST_BIT
