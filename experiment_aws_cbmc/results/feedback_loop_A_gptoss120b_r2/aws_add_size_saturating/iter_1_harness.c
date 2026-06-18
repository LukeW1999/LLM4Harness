#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Maximum buffer size for bounding the nondeterministic allocation */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 256
#endif

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    uint8_t *pBuf = NULL;

    /* Allocate a buffer only when size is non‑zero */
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);

        /* Initialize buffer with nondeterministic data */
        for (size_t i = 0; i < bufsize; ++i) {
            pBuf[i] = nondet_uint8_t();
        }
    }

    /* 2. Save old state of the buffer (if any) */
    uint8_t *old = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old = malloc(bufsize);
        __CPROVER_assume(old != NULL);
        memcpy(old, pBuf, bufsize);
    }

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Postconditions */

    /* Pointer itself must remain unchanged */
    assert((pBuf == NULL && bufsize == 0) || (pBuf != NULL));

    if (pBuf != NULL && bufsize != 0) {
        /* Success path: all bytes must be zero */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    } else {
        /* Failure/early‑return path: buffer must be unchanged */
        if (old != NULL && bufsize > 0) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(pBuf[i] == old[i]);
            }
        }
    }

    /* 5. Clean up */
    if (pBuf) {
        free(pBuf);
    }
    if (old) {
        free(old);
    }
}
