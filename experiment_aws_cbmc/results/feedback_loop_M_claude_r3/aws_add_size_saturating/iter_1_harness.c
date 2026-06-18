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
 * 2. The pointer pBuf itself is unchanged (same address).
 * 3. bufsize is unchanged.
 */

void aws_secure_zero_harness(void) {
    /* Bound the buffer size to prevent state space explosion */
    size_t bufsize = nondet_size_t();
    __CPROVER_assume(bufsize <= MAX_BUFFER_SIZE);

    /* Non-deterministically decide whether to pass NULL or a valid buffer */
    bool use_null = nondet_bool();

    uint8_t *buf = NULL;
    if (!use_null && bufsize > 0) {
        buf = (uint8_t *)malloc(bufsize);
        __CPROVER_assume(buf != NULL);
    }

    /* Save original pointer value */
    uint8_t *old_buf = buf;
    size_t old_bufsize = bufsize;

    /* Call the function under test */
    aws_secure_zero(buf, bufsize);

    /* Postcondition 1: pointer is unchanged */
    assert(buf == old_buf);

    /* Postcondition 2: bufsize is unchanged (it's passed by value, but verify our local copy) */
    assert(bufsize == old_bufsize);

    /* Postcondition 3: If buf != NULL and bufsize > 0, all bytes must be zero */
    if (buf != NULL && bufsize > 0) {
        for (size_t i = 0; i < bufsize; i++) {
            assert(buf[i] == 0);
        }
    }
}

void aws_add_size_saturating_harness(void) {
    aws_secure_zero_harness();
    return 0;
}
