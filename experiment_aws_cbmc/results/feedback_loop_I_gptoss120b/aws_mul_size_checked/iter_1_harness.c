#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

/* The harness for aws_secure_zero */
void aws_secure_zero_harness(void) {
    /* nondeterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide whether the buffer pointer is NULL */
    bool ptr_is_null = nondet_bool();

    void *pBuf;
    uint8_t *old_contents = NULL;

    if (!ptr_is_null) {
        /* allocate a buffer that is at least MAX_BUFFER_SIZE bytes long */
        pBuf = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(pBuf != NULL);

        /* allocate storage for the original contents */
        old_contents = malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(old_contents != NULL);

        /* copy the original contents byte‑by‑byte */
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            ((uint8_t *)pBuf)[i] = nondet_uint8_t();
            old_contents[i] = ((uint8_t *)pBuf)[i];
        }
    } else {
        pBuf = NULL;
    }

    /* Save old state for later comparison */
    void *old_pBuf = pBuf;
    size_t old_bufsize = bufsize;

    /* Call the function under verification */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑condition checks */
    if (pBuf != NULL && bufsize > 0) {
        /* The first bufsize bytes must be zero */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(((uint8_t *)pBuf)[i] == 0);
        }
        /* Bytes beyond bufsize must remain unchanged */
        for (size_t i = bufsize; i < MAX_BUFFER_SIZE; ++i) {
            assert(((uint8_t *)pBuf)[i] == old_contents[i]);
        }
    } else {
        /* When the pointer is NULL or size is zero, the memory must be unchanged */
        if (pBuf != NULL) {
            for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
                assert(((uint8_t *)pBuf)[i] == old_contents[i]);
            }
        }
        /* No memory to check when pBuf is NULL */
    }

    /* The pointer itself and the size argument must not be altered */
    assert(pBuf == old_pBuf);
    assert(bufsize == old_bufsize);
}
