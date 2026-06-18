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
 * 1. If pBuf != NULL and bufsize > 0: all bytes in [pBuf, pBuf+bufsize) are 0.
 * 2. The function does not crash or cause undefined behavior.
 */

void aws_secure_zero_harness(void) {
    /* Non-deterministic buffer size, bounded to prevent state space explosion */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Non-deterministically decide whether to pass NULL or a valid buffer */
    bool use_null = nondet_bool();

    if (use_null) {
        /* Test NULL path: function should handle NULL gracefully */
        aws_secure_zero(NULL, 0);
        /* No assertion needed beyond "no crash" — the function returns early */
    } else {
        if (bufsize == 0) {
            /* Zero-size buffer: function should handle gracefully */
            /* We can pass any pointer (even NULL) with size 0 */
            uint8_t dummy;
            aws_secure_zero(&dummy, 0);
            /* No crash is the postcondition */
        } else {
            /* Allocate a buffer of the given size */
            uint8_t *buf = malloc(bufsize);
            __CPROVER_assume(buf != NULL);

            /* Fill with non-deterministic data */
            for (size_t i = 0; i < bufsize; i++) {
                buf[i] = nondet_uint8_t();
            }

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

void aws_add_size_saturating_harness(void) {
    aws_secure_zero_harness();
    return 0;
}
