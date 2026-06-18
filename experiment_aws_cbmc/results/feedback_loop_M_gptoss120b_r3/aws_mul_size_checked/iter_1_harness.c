#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    bool is_null = nondet_bool();
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    void *pBuf = NULL;
    uint8_t *byte_buf = NULL;          /* convenient typed view */
    uint8_t *old_buf = NULL;           /* snapshot of original contents */

    if (is_null) {
        pBuf = NULL;
        /* The implementation asserts bufsize == 0 when pBuf is NULL */
        __CPROVER_assume(bufsize == 0);
    } else {
        /* Allocate a buffer of the chosen size (may be zero) */
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            byte_buf = (uint8_t *)pBuf;

            /* Fill the buffer with nondeterministic data */
            for (size_t i = 0; i < bufsize; ++i) {
                byte_buf[i] = nondet_uint8_t();
            }

            /* Save a copy of the original contents for later comparison */
            old_buf = malloc(bufsize);
            __CPROVER_assume(old_buf != NULL);
            for (size_t i = 0; i < bufsize; ++i) {
                old_buf[i] = byte_buf[i];
            }
        } else {
            /* bufsize == 0, pBuf may be any non‑NULL pointer (or NULL);
               the function will return early without touching memory. */
            pBuf = malloc(1);               /* allocate at least 1 byte to have a valid pointer */
            __CPROVER_assume(pBuf != NULL);
            byte_buf = (uint8_t *)pBuf;
            /* No need to initialise contents when size is zero */
        }
    }

    /* 2. Save old state of scalar inputs */
    size_t old_bufsize = bufsize;
    void *old_pBuf = pBuf;

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Post‑condition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* On success the buffer must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(byte_buf[i] == 0);
        }
    } else {
        /* When the function returns early (pBuf NULL or size 0) the memory must be unchanged */
        if (bufsize > 0 && pBuf != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(byte_buf[i] == old_buf[i]);
            }
        }
    }

    /* 5. Unchanged scalar fields */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    /* 6. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_buf != NULL) {
        free(old_buf);
    }
}
