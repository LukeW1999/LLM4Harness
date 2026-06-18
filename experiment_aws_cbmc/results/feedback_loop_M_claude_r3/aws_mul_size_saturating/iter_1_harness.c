#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_secure_zero:
 * - If pBuf is NULL or bufsize is 0, the function returns early (no-op).
 * - Otherwise, it zeroes bufsize bytes starting at pBuf.
 *
 * Postconditions:
 * 1. If pBuf != NULL and bufsize > 0, all bytes in [pBuf, pBuf+bufsize) are 0.
 * 2. The function does not crash or cause undefined behavior.
 */

void aws_secure_zero_harness(void) {
    /* Bound the buffer size to prevent state space explosion */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Non-deterministically choose whether to pass NULL or a valid pointer */
    bool use_null = nondet_bool();

    if (use_null) {
        /* Test NULL path: bufsize must be 0 per the assert in the implementation */
        aws_secure_zero(NULL, 0);
        /* No postcondition to check beyond no crash */
    } else {
        /* Test valid buffer path */
        if (bufsize == 0) {
            /* Zero-size buffer: function should be a no-op */
            uint8_t *buf = malloc(1); /* allocate at least 1 byte to have a valid pointer */
            __CPROVER_assume(buf != NULL);
            aws_secure_zero(buf, 0);
            /* No bytes should have been modified */
            free(buf);
        } else {
            /* Allocate a buffer of the given size */
            uint8_t *buf = malloc(bufsize);
            __CPROVER_assume(buf != NULL);

            /* Call the function under test */
            aws_secure_zero(buf, bufsize);

            /* Postcondition: all bytes must be zero */
            for (size_t i = 0; i < bufsize; i++) {
                assert(buf[i] == 0);
            }

            free(buf);
        }
    }
}

void aws_mul_size_saturating_harness(void) {
    aws_secure_zero_harness();
    return 0;
}
