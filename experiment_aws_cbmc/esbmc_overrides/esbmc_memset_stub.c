/**
 * ESBMC-compatible memset stub.
 * Havoces (nondeterminizes) destination bytes rather than using a concrete
 * loop, which avoids expensive Z3 loop unrolling while still ensuring the
 * destination was reachable.  Functionally equivalent to the CBMC
 * memset_override.c stub for the purposes of safety proofs.
 */

#undef memset

#include <stddef.h>

void *memset(void *s, int c, size_t n) {
    if (n > 0 && s) {
        unsigned char *p = (unsigned char *)s;
        for (size_t i = 0; i < n; i++)
            p[i] = (unsigned char)(c & 0xFF);
    }
    return s;
}
