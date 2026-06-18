#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* 2. Non‑deterministic pointer (NULL or allocated) */
    uint8_t *pBuf;
    bool is_null = nondet_bool();
    if (is_null) {
        pBuf = NULL;
    } else {
        /* allocate at least bufsize bytes (malloc may return NULL, so assume success) */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* ensure the allocated region is writable */
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(pBuf, bufsize));
    }

    /* 3. Save old state */
    uint8_t *old_buf = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_buf = malloc(bufsize);
        __CPROVER_assume(old_buf != NULL);
        memcpy(old_buf, pBuf, bufsize);
    }
    uint8_t *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* 4. Call function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Post‑conditions */

    /* pointer and size themselves must remain unchanged */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    if (pBuf != NULL && bufsize != 0) {
        /* on success the buffer must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    } else {
        /* on early‑return the buffer must be unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* zero‑length buffer – nothing to check, but ensure no modification */
            /* (no bytes to compare) */
        } else if (pBuf != NULL && bufsize > 0) {
            /* this case cannot happen because the function would have zeroed the buffer */
        } else {
            /* pBuf == NULL – nothing to check */
        }
        if (old_buf != NULL && bufsize > 0) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(pBuf[i] == old_buf[i]);
            }
        }
    }

    /* 6. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_buf != NULL) {
        free(old_buf);
    }
}
