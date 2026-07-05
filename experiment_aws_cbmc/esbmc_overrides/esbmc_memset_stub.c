/**
 * ESBMC-compatible memset stub.
 * Reinstates the safety contract that AWS's official CBMC memset_override.c
 * checks (dest-writable) via an explicit precondition, using ESBMC's native
 * __ESBMC_r_ok as an approximation for w_ok (no ESBMC-native w_ok exists).
 */

#undef memset

#include <stddef.h>

void *memset(void *s, int c, size_t n) {
    __ESBMC_assert(__ESBMC_r_ok(s, n), "memset destination region writeable (approximated via r_ok)");
    if (n > 0) {
        unsigned char *p = (unsigned char *)s;
        for (size_t i = 0; i < n; i++)
            p[i] = (unsigned char)(c & 0xFF);
    }
    return s;
}
