/**
 * ESBMC-compatible memcpy stub.
 * Provides a simple byte-by-byte copy to replace ESBMC's internal __memcpy_impl
 * loop, avoiding deep loop unwinding and Z3 blowup on symbolic buffer sizes.
 */

#undef memcpy

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}
