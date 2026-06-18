#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* Prototype of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool allocate = nondet_bool();
    void *pBuf = NULL;

    if (allocate) {
        /* Allocate a buffer of the chosen size */
        pBuf = malloc(bufsize);
        /* If allocation fails, it must be because size is zero */
        __CPROVER_assume(pBuf != NULL || bufsize == 0);
    } else {
        /* When we do not allocate, the pointer must be NULL and size must be zero
           to avoid the internal AWS_ASSERT(bufsize == 0) in the function. */
        pBuf = NULL;
        __CPROVER_assume(bufsize == 0);
    }

    /* 2. Save old state */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Post‑condition checks */

    /* The pointer and size arguments must remain unchanged */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf != NULL && bufsize > 0) {
        /* The buffer must be zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* When the buffer is NULL, the size must be zero (ensured by the assumption above) */
        if (pBuf == NULL) {
            assert(bufsize == 0);
        }
        /* When size is zero, there is nothing to check */
    }
}
