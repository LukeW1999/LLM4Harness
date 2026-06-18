#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>

/* Prototype of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Allocate a bounded buffer */
    size_t max_size = MAX_BUFFER_SIZE;
    uint8_t *buf = malloc(max_size);
    __CPROVER_assume(buf != NULL);

    /* 2. Fill the buffer with nondeterministic data */
    for (size_t i = 0; i < max_size; ++i) {
        buf[i] = nondet_uint8_t();
    }

    /* 3. Save a copy of the original contents */
    uint8_t *old = malloc(max_size);
    __CPROVER_assume(old != NULL);
    for (size_t i = 0; i < max_size; ++i) {
        old[i] = buf[i];
    }

    /* 4. Choose a nondeterministic pointer and size */
    void *pBuf = nondet_bool() ? NULL : (void *)buf;
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= max_size);

    /* Precondition derived from the function's implementation:
     * If pBuf is NULL then bufsize must be 0 (AWS_ASSERT). */
    __CPROVER_assume(pBuf != NULL || bufsize == 0);

    /* 5. Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* 6. Post‑condition checks */
    if (pBuf != NULL && bufsize != 0) {
        /* Bytes that should have been zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }
        /* Bytes beyond bufsize must remain unchanged */
        for (size_t i = bufsize; i < max_size; ++i) {
            assert(buf[i] == old[i]);
        }
    } else {
        /* No modification should have occurred */
        for (size_t i = 0; i < max_size; ++i) {
            assert(buf[i] == old[i]);
        }
    }

    /* 7. Clean up */
    free(old);
    free(buf);
}
