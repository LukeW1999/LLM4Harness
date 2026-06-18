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
    /* Non‑deterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Non‑deterministic pointer: either NULL or a malloc’ed buffer */
    void *pBuf;
    if (nondet_bool()) {
        pBuf = NULL;
    } else {
        pBuf = malloc(bufsize);
        /* If malloc fails we treat it as a zero‑length buffer – the function
         * only accesses memory when bufsize > 0, so this is safe. */
        __CPROVER_assume(pBuf != NULL || bufsize == 0);
    }

    /* The function asserts that when pBuf is NULL the size must be zero. */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    /* Save old state for later comparison */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    uint8_t *old_contents = NULL;
    if (pBuf != NULL && bufsize > 0) {
        old_contents = malloc(bufsize);
        __CPROVER_assume(old_contents != NULL);
        /* Initialise the buffer with nondeterministic data */
        memcpy(old_contents, pBuf, bufsize);
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑conditions */
    if (old_pBuf != NULL && old_bufsize != 0) {
        /* Successful zeroing: every byte must be 0 */
        for (size_t i = 0; i < old_bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
    } else {
        /* No zeroing performed: memory must be unchanged */
        if (old_contents != NULL) {
            for (size_t i = 0; i < old_bufsize; ++i) {
                assert(((uint8_t *)pBuf)[i] == old_contents[i]);
            }
        }
    }

    /* Fields that must never change */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);

    /* Clean up */
    free(old_contents);
    free(pBuf);
}
