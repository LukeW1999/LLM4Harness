#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_secure_zero_harness(void) {
    /* nondeterministic size, bounded */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* nondeterministically decide whether the pointer is NULL */
    bool ptr_is_null = nondet_bool();

    uint8_t *pBuf = NULL;
    uint8_t *guard_before = NULL;
    uint8_t *guard_after = NULL;
    uint8_t *old_buf = NULL;
    uint8_t old_guard_before = 0;
    uint8_t old_guard_after = 0;

    if (!ptr_is_null) {
        /* allocate guard bytes + buffer */
        size_t total = bufsize + 2;               /* one guard byte before and after */
        uint8_t *mem = malloc(total);
        __CPROVER_assume(mem != NULL);

        guard_before = mem;                       /* first byte is guard before */
        pBuf = mem + 1;                           /* buffer starts after guard */
        guard_after = mem + 1 + bufsize;          /* guard after buffer */

        /* initialize guard bytes nondeterministically */
        *guard_before = nondet_uint8_t();
        *guard_after  = nondet_uint8_t();

        /* save old guard values */
        old_guard_before = *guard_before;
        old_guard_after  = *guard_after;

        /* initialize buffer nondeterministically */
        for (size_t i = 0; i < bufsize; ++i) {
            pBuf[i] = nondet_uint8_t();
        }

        /* keep a copy of the original buffer for failure case comparison */
        if (bufsize > 0) {
            old_buf = malloc(bufsize);
            __CPROVER_assume(old_buf != NULL);
            for (size_t i = 0; i < bufsize; ++i) {
                old_buf[i] = pBuf[i];
            }
        }
    }

    /* Call the function under test */
    aws_secure_zero(pBuf, bufsize);

    /* Post‑conditions */
    if (pBuf != NULL && bufsize > 0) {
        /* Successful zeroing: every byte in the buffer must be 0 */
        for (size_t i = 0; i < bufsize; ++i) {
            assert(pBuf[i] == 0);
        }
    } else {
        /* Failure path: buffer (if any) must be unchanged */
        if (pBuf != NULL && bufsize == 0) {
            /* nothing to check inside the buffer */
        } else if (pBuf == NULL) {
            /* nothing to check */
        }
        if (old_buf != NULL) {
            for (size_t i = 0; i < bufsize; ++i) {
                assert(pBuf[i] == old_buf[i]);
            }
        }
    }

    /* Guard bytes must never be altered */
    if (guard_before != NULL) {
        assert(*guard_before == old_guard_before);
    }
    if (guard_after != NULL) {
        assert(*guard_after == old_guard_after);
    }

    /* Clean up */
    free(old_buf);
    if (!ptr_is_null) {
        free(guard_before); /* guard_before points to the original malloc block */
    }
}
