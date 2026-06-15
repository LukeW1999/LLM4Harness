#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "proof_helpers/make_common_data_structures.h"

/* Function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    uint8_t *buf = NULL;
    uint8_t *old = NULL;

    if (bufsize > 0) {
        /* Allocate a buffer of the chosen size */
        buf = malloc(bufsize);
        __CPROVER_assume(buf != NULL);

        /* Fill the buffer with nondeterministic data */
        for (size_t i = 0; i < bufsize; ++i) {
            buf[i] = nondet_uint8_t();
        }

        /* Save a copy of the original contents for later comparison */
        old = malloc(bufsize);
        __CPROVER_assume(old != NULL);
        memcpy(old, buf, bufsize);
    } else {
        /* When size is zero the pointer may be NULL or non‑NULL; both are allowed */
        buf = nondet_uint8_t() ? NULL : (uint8_t *)malloc(1);
        /* If we allocated a dummy byte for the zero‑size case, initialise it */
        if (buf != NULL) {
            buf[0] = nondet_uint8_t();
        }
    }

    /* 2. Call the function under test */
    aws_secure_zero(buf, bufsize);

    /* 3. Post‑conditions */

    if (buf != NULL && bufsize > 0) {
        /* Success path: the region of length bufsize must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }
    } else if (buf != NULL && bufsize == 0) {
        /* Failure path (size zero): buffer must remain unchanged */
        for (size_t i = 0; i < 1; ++i) { /* only the dummy byte we may have allocated */
            assert(buf[i] == old[0]);
        }
    } else {
        /* buf == NULL: nothing to check, just ensure we did not dereference */
        /* No memory to assert */
    }

    /* 4. Clean up */
    if (buf != NULL && bufsize > 0) {
        free(buf);
        free(old);
    } else if (buf != NULL && bufsize == 0) {
        free(buf);
        free(old);
    }
}
