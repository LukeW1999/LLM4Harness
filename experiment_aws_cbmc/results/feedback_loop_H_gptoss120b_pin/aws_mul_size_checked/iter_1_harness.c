#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

/* Assume MAX_BUFFER_SIZE is defined by the proof environment */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool pbuf_is_null = nondet_bool();
    void *pBuf = NULL;

    /* 2. Allocate buffer if not NULL */
    if (!pbuf_is_null) {
        /* Allocate a buffer of the given size */
        pBuf = malloc(bufsize);
        /* Ensure allocation succeeded when size > 0 */
        __CPROVER_assume(pBuf != NULL || bufsize == 0);

        /* Initialize the buffer with nondeterministic data */
        if (bufsize > 0) {
            uint8_t *buf = (uint8_t *)pBuf;
            for (size_t i = 0; i < bufsize; ++i) {
                buf[i] = nondet_uint8_t();
            }
        }
    } else {
        pBuf = NULL;
        /* Precondition: if pBuf is NULL then bufsize must be 0 (matches AWS_ASSERT) */
        __CPROVER_assume(bufsize == 0);
    }

    /* 3. Save old state of the buffer for later comparison (if needed) */
    uint8_t *old_contents = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_contents = malloc(bufsize);
        __CPROVER_assume(old_contents != NULL);
        memcpy(old_contents, pBuf, bufsize);
    }

    /* 4. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 5. Assert postconditions */
    if (pBuf != NULL && bufsize > 0) {
        uint8_t *buf = (uint8_t *)pBuf;
        for (size_t i = 0; i < bufsize; ++i) {
            /* All bytes must be zero after the call */
            assert(buf[i] == 0);
        }
    } else {
        /* When pBuf is NULL or bufsize is 0, the function returns early.
         * No memory is modified; nothing to assert beyond the precondition already assumed. */
        assert(pBuf == NULL || bufsize == 0);
    }

    /* 6. Clean up */
    if (pBuf) {
        free(pBuf);
    }
    if (old_contents) {
        free(old_contents);
    }
}
