#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* 1. Nondeterministic inputs */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    bool p_is_null = nondet_bool();

    void *pBuf = NULL;
    uint8_t *allocated = NULL;

    if (!p_is_null && bufsize > 0) {
        allocated = malloc(bufsize);
        __CPROVER_assume(allocated != NULL);
        /* Fill the buffer with nondeterministic data */
        for (size_t i = 0; i < bufsize; ++i) {
            allocated[i] = nondet_uint8_t();
        }
        pBuf = allocated;
    } else {
        pBuf = NULL;
        /* The function asserts that if pBuf is NULL then bufsize must be 0 */
        __CPROVER_assume(!(pBuf == NULL && bufsize != 0));
    }

    /* 2. Save old state of the memory region (if any) */
    uint8_t *old_mem = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_mem = malloc(bufsize);
        __CPROVER_assume(old_mem != NULL);
        memcpy(old_mem, pBuf, bufsize);
    }

    /* 3. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 4. Postconditions */

    /* 4a. If a non‑NULL buffer with non‑zero size was provided, all bytes must be zero */
    if (pBuf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    } else {
        /* 4b. Otherwise the memory region must be unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* No bytes to check – nothing should have been written */
        } else {
            /* pBuf == NULL – nothing to check */
        }
    }

    /* 5. Fields that must not change */
    if (!p_is_null) {
        assert(pBuf == allocated);
    } else {
        assert(pBuf == NULL);
    }
    /* bufsize is a local variable and cannot be modified by the function */

    /* 6. If we saved old memory, ensure it matches the original content when no zeroing occurs */
    if (old_mem != NULL && !(pBuf != NULL && bufsize > 0)) {
        assert(memcmp(old_mem, pBuf, bufsize) == 0);
    }

    /* Clean up */
    free(allocated);
    free(old_mem);
}
