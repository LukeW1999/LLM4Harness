/**
 * ESBMC-compatible memcpy stub.
 * Provides a simple byte-by-byte copy to replace ESBMC's internal __memcpy_impl
 * loop, avoiding deep loop unwinding and Z3 blowup on symbolic buffer sizes.
 *
 * Also reinstates the safety contract that AWS's official CBMC memcpy_override.c
 * checks (overlap / source-readable / dest-writable) via explicit preconditions,
 * using ESBMC-native builtins (__ESBMC_same_object, __ESBMC_r_ok). These were
 * silently dropped in the original port, making ESBMC blind to mutants whose
 * only observable effect is an overlapping or out-of-bounds memcpy. w_ok has no
 * ESBMC-native equivalent, so the destination-writable check is approximated
 * with __ESBMC_r_ok (not exact parity with CBMC's w_ok).
 */

#undef memcpy

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n) {
    __ESBMC_assert(
        !__ESBMC_same_object(dest, src) ||
            ((const char *)src >= (const char *)dest + n) ||
            ((const char *)dest >= (const char *)src + n),
        "memcpy src/dst overlap");
    /* __ESBMC_r_ok(addr,len) is vacuously true when addr==NULL && len==0 (it
     * encodes addr==0 => len==0), unlike CBMC's w_ok/r_ok which AWS's own
     * memcpy_override_havoc.c additionally guards with an explicit `!= NULL`
     * conjunct. Match that exact CBMC convention here. */
    __ESBMC_assert(src != NULL && __ESBMC_r_ok((void *)src, n), "memcpy source region readable");
    __ESBMC_assert(dest != NULL && __ESBMC_r_ok(dest, n), "memcpy destination region writeable (approximated via r_ok)");

    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
    return dest;
}
