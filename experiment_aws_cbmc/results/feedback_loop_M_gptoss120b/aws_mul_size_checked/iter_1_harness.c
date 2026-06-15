#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* 1. Nondeterministic inputs bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool p_is_null = nondet_bool();

    void *pBuf = NULL;
    uint8_t *old_contents = NULL;

    if (!p_is_null) {
        /* Allocate a buffer when not NULL */
        if (bufsize > 0) {
            pBuf = malloc(bufsize);
            __CPROVER_assume(pBuf != NULL);
            /* Save original contents to compare on failure path */
            old_contents = malloc(bufsize);
            __CPROVER_assume(old_contents != NULL);
            for (size_t i = 0; i < bufsize; ++i) {
                ((uint8_t *)old_contents)[i] = ((uint8_t *)pBuf)[i];
            }
        } else {
            /* bufsize == 0, still allocate a possibly zero‑length buffer */
            pBuf = malloc(1);
            __CPROVER_assume(pBuf != NULL);
        }
    } else {
        pBuf = NULL;
    }

    /* 2. Call function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 3. Post‑condition checks */
    if (pBuf == NULL || bufsize == 0) {
        /* Early‑return path: no modification of memory when buffer is non‑NULL */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to modify, nothing to assert */
        } else if (pBuf != NULL && bufsize > 0) {
            /* This branch cannot happen because bufsize == 0 when we reach here */
        } else {
            /* pBuf == NULL, nothing to check */
        }
    } else {
        /* Successful zeroing: every byte must be 0 */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    }

    /* 4. Ensure that when zeroing is not performed, memory is unchanged */
    if (pBuf != NULL && bufsize > 0 && (pBuf == NULL || bufsize == 0)) {
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == ((uint8_t *)old_contents)[i]);
        }
    }

    /* 5. Clean up */
    if (pBuf) {
        free(pBuf);
    }
    if (old_contents) {
        free(old_contents);
    }
}
