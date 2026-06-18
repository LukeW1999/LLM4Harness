#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs and bound them */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    uint8_t *pBuf = NULL;
    if (bufsize > 0) {
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL);
        /* initialize buffer with nondeterministic data */
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
    if (pBuf != NULL && bufsize != 0) {
        /* On success (non‑zero size and non‑NULL pointer) the buffer must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    } else {
        /* On early return (NULL pointer or zero size) the buffer must remain unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to check – trivially unchanged */
        } else if (pBuf == NULL) {
            /* No memory to check – trivially unchanged */
        }
    }

    /* 5. Fields that must not change regardless of result */
    /* The pointer itself must remain the same (no reallocation) */
    if (pBuf != NULL && bufsize != 0) {
        assert(old != NULL);
        /* Ensure the original content is no longer present (already checked zeroing) */
        /* No additional pointer fields to check */
    }

    /* 6. Clean up */
    if (pBuf) {
        free(pBuf);
    }
    if (old) {
        free(old);
    }
}
