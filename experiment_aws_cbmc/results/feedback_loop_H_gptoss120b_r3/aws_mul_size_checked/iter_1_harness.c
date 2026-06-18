#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic buffer size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide whether the pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    uint8_t *old_bytes = NULL;

    if (ptr_is_null) {
        pBuf = NULL;
        /* The function asserts that bufsize == 0 when pBuf is NULL */
        __CPROVER_assume(bufsize == 0);
    } else {
        /* Allocate a buffer of the given size (may be zero) */
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            /* Fill the buffer with nondeterministic data */
            for (size_t i = 0; i < bufsize; ++i) {
                ((uint8_t *)pBuf)[i] = nondet_uint8_t();
            }
            /* Save a copy of the original contents for later comparison */
            old_bytes = malloc(bufsize);
            __CPROVER_assume(old_bytes != NULL);
            memcpy(old_bytes, pBuf, bufsize);
        } else {
            /* bufsize == 0, pointer may be any non‑NULL value; we keep it NULL for simplicity */
            pBuf = NULL;
        }
    }

    /* Call the function under verification */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑condition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* All bytes must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    } else {
        /* When the function returns early (NULL pointer or zero size) the memory must be unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to check – vacuously true */
        } else if (pBuf != NULL && old_bytes != NULL) {
            /* bufsize == 0 case already handled; this block is unreachable but kept for completeness */
            for (size_t i = 0; i < bufsize; ++i) {
                assert(((uint8_t *)pBuf)[i] == old_bytes[i]);
            }
        }
    }

    /* Clean up */
    free(old_bytes);
    free(pBuf);
}
