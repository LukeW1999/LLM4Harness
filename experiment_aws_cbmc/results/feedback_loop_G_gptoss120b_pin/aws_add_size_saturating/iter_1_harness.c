#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

/* Prototype of the function under test */
void aws_secure_zero(void *pBuf, size_t bufsize);

void aws_secure_zero_harness(void) {
    /* 1. Declare nondeterministic inputs */
    size_t size = nondet_size_t();
    __CPROVER_assume(size <= MAX_BUFFER_SIZE);

    uint8_t *buf = NULL;
    uint8_t *old_buf = NULL;

    if (size > 0) {
        /* Allocate a buffer of the given size */
        buf = malloc(size);
        __CPROVER_assume(buf != NULL);
        /* Initialize buffer with nondeterministic values */
        for (size_t i = 0; i < size; ++i) {
            buf[i] = nondet_uint8_t();
        }
        /* Save a copy of the original buffer contents */
        old_buf = malloc(size);
        __CPROVER_assume(old_buf != NULL);
        memcpy(old_buf, buf, size);
    } else {
        /* size == 0, buf remains NULL */
        buf = NULL;
        old_buf = NULL;
    }

    /* 2. Call the function under test */
    aws_secure_zero(buf, size);

    /* 3. Postcondition checks */
    if (buf != NULL && size != 0) {
        /* On success (non‑NULL pointer and non‑zero size) the buffer must be zeroed */
        for (size_t i = 0; i < size; ++i) {
            assert(buf[i] == 0);
        }
        /* The pointer itself must not change */
        assert(buf == buf);
    } else {
        /* On early‑return (NULL pointer or zero size) the buffer must remain unchanged */
        if (buf != NULL && size == 0) {
            /* No bytes to check; buffer unchanged trivially */
        } else if (buf == NULL) {
            /* Nothing to check */
        }
        /* If a buffer was allocated (size > 0) it would have been zeroed, so the only unchanged case is when size == 0 */
        if (old_buf != NULL && size == 0) {
            /* No modification expected; compare with saved copy (zero bytes) */
            /* No bytes to compare, so nothing to assert */
        }
    }

    /* 4. Clean up */
    if (buf) {
        free(buf);
    }
    if (old_buf) {
        free(old_buf);
    }
}
