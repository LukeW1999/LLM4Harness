#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Decide whether the pointer is NULL or points to a writable buffer */
    void *pBuf;
    if (nondet_bool()) {
        pBuf = NULL;
    } else {
        /* Allocate a buffer of the chosen size (may be zero) */
        pBuf = malloc(bufsize);
        __CPROVER_assume(pBuf != NULL || bufsize == 0);
    }

    /* 2. Save old memory state when a buffer is present */
    unsigned char *old_mem = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_mem = malloc(bufsize);
        __CPROVER_assume(old_mem != NULL);
        memcpy(old_mem, pBuf, bufsize);
    }

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Post‑conditions */

    /* If the function returned early (NULL pointer or zero size) */
    if (pBuf == NULL || bufsize == 0) {
        /* The implementation asserts that bufsize is zero in this case */
        assert(bufsize == 0);
        /* No memory should have been modified */
        if (pBuf != NULL && old_mem != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(((unsigned char *)pBuf)[i] == old_mem[i]);
            }
        }
    } else {
        /* Normal path: the buffer must be completely zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((unsigned char *)pBuf)[i] == 0);
        }
        /* The pointer itself must remain unchanged */
        assert(pBuf != NULL);
    }

    /* 5. Clean up */
    if (pBuf != NULL) {
        free(pBuf);
    }
    if (old_mem != NULL) {
        free(old_mem);
    }
}
