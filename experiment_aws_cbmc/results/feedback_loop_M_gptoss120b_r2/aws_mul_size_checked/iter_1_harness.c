#include <aws/common/byte_buf.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool make_null = nondet_bool();

    void *pBuf = NULL;
    uint8_t *old_contents = NULL;

    if (make_null) {
        /* pBuf is NULL – per function contract bufsize must be 0 */
        __CPROVER_assume(bufsize == 0);
        pBuf = NULL;
    } else {
        /* Allocate a buffer of the chosen size */
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            /* Fill with nondeterministic data */
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                bytes[i] = nondet_uint8_t();
            }
            /* Save a copy for later immutability checks (used when bufsize == 0) */
            old_contents = malloc(bufsize);
            __CPROVER_assume(old_contents != NULL);
            memcpy(old_contents, pBuf, bufsize);
        } else {
            /* bufsize == 0 – pBuf may be NULL or a zero‑length allocation */
            pBuf = malloc(0);
            /* No need to save old contents when size is zero */
        }
    }

    /* 2. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition assertions */
    if (pBuf == NULL) {
        /* When the buffer pointer is NULL the function requires bufsize == 0 */
        assert(bufsize == 0);
    } else if (bufsize == 0) {
        /* No bytes to zero – the buffer must remain unchanged */
        if (old_contents != NULL) {
            uint8_t *bytes = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                assert(bytes[i] == old_contents[i]);
            }
        }
    } else {
        /* All bytes must be zero after a successful zeroing */
        uint8_t *bytes = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            assert(bytes[i] == 0);
        }
    }

    /* 4. Clean up */
    if (pBuf) {
        free(pBuf);
    }
    if (old_contents) {
        free(old_contents);
    }
}
