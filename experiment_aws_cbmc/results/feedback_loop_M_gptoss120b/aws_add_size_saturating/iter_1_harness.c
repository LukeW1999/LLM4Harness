#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Nondeterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Decide whether the buffer pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf = NULL;
    void *old_pBuf = NULL;

    if (!ptr_is_null) {
        /* Allocate a buffer of exactly bufsize bytes (may be zero) */
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            /* Initialize memory with nondeterministic values */
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                bytes[i] = nondet_uint8_t();
            }
        } else {
            /* bufsize == 0, still allocate a non‑NULL pointer (allowed) */
            pBuf = malloc(1);
            __CPROVER_assume(pBuf != NULL);
        }
    } else {
        pBuf = NULL;
    }

    old_pBuf = pBuf; /* save pointer value */

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition checks */

    /* The function asserts that if either argument is “invalid” (NULL pointer or zero size)
       then the size must be zero.  Therefore after the call we must not have a NULL pointer
       with a non‑zero size. */
    assert(!(pBuf == NULL && bufsize != 0));

    /* Pointer itself must remain unchanged */
    assert(pBuf == old_pBuf);

    if (pBuf != NULL && bufsize > 0) {
        /* All bytes in the buffer must be zero after the call */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* When bufsize == 0 the buffer (if any) is untouched; no further checks needed */
        /* When pBuf == NULL the function returns early; nothing to check. */
    }

    /* 4. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
}
