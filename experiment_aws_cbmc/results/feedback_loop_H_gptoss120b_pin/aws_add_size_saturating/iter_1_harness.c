#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Helper struct defined in proof_helpers for storing a snapshot of a byte array */
struct store_byte_from_buffer;

/* The function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();

    void *pBuf;
    void *old_pBuf = NULL;
    struct store_byte_from_buffer old_bytes;

    if (make_null) {
        pBuf = NULL;
    } else {
        /* Allocate a buffer of the chosen size (may be NULL if malloc fails) */
        pBuf = malloc(bufsize);
        old_pBuf = pBuf;

        if (pBuf != NULL && bufsize > 0) {
            /* Save a snapshot of the original contents for later comparison */
            save_byte_from_array((const uint8_t *)pBuf, bufsize, &old_bytes);
        }
    }

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition checks */

    /* The pointer argument must remain unchanged */
    assert(pBuf == old_pBuf);

    if (pBuf != NULL && bufsize > 0) {
        /* On success path the memory region must be zeroed */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    } else {
        /* When pBuf is NULL or bufsize is 0 the function returns early;
         * the memory (if any) must remain unchanged. */
        if (old_pBuf != NULL && bufsize > 0) {
            assert_byte_from_buffer_matches((const uint8_t *)old_pBuf, &old_bytes);
        }
    }

    /* 4. Clean up */
    free(pBuf);
}
