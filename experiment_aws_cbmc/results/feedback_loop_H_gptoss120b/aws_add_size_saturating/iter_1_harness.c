#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "aws/common/common.h"
#include "proof_helpers/make_common_data_structures.h"

/* Function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Non‑deterministic inputs, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    uint8_t *buf = NULL;
    if (bufsize == 0) {
        /* When size is zero the pointer may be NULL – the implementation asserts this */
        buf = NULL;
    } else {
        /* The implementation asserts that a non‑NULL pointer is provided when size > 0 */
        buf = malloc(bufsize);
        __CPROVER_assume(buf != NULL);
    }
    __CPROVER_assume(buf != NULL || bufsize == 0);   /* precondition required by the function */

    /* 2. Initialise memory with nondeterministic values */
    uint8_t *old_buf = NULL;
    if (buf != NULL && bufsize > 0) {
        old_buf = malloc(bufsize);
        __CPROVER_assume(old_buf != NULL);
        for (size_t i = 0; i < bufsize; ++i) {
            buf[i] = nondet_uint8_t();
            old_buf[i] = buf[i];
        }
    }

    /* 3. Call the function under test */
    aws_secure_zero(buf, bufsize);

    /* 4. Post‑conditions */
    if (buf != NULL && bufsize > 0) {
        /* Success path – the buffer must be zeroed */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(buf[i] == 0);
        }
    } else {
        /* Failure / early‑return path – the buffer must be unchanged */
        if (old_buf != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(buf[i] == old_buf[i]);
            }
        }
    }

    /* 5. Fields that must never change */
    /* The pointer itself and the size are passed by value; they cannot be altered by the function.
       No additional structure fields exist, so no further unchanged‑field asserts are needed. */

    /* 6. Clean up */
    free(old_buf);
    free(buf);
}
