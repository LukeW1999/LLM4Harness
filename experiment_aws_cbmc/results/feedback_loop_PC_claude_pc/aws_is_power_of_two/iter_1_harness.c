#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_round_up_to_power_of_two:
 *
 * Doxygen: "Function to find the smallest result that is power of 2 >= n.
 *           Returns AWS_OP_ERR if this cannot be done without overflow."
 *
 * Analysis:
 * 1. Changed fields on success: *result is set to the smallest power of 2 >= n.
 * 2. Unchanged fields: n is not modified (input only).
 * 3. Failure path: returns AWS_OP_ERR when n > SIZE_MAX_POWER_OF_TWO; *result is not written.
 * 4. Validity invariants:
 *    - On success, *result >= n
 *    - On success, *result is a power of two (aws_is_power_of_two(*result))
 *    - On success with n == 0, *result == 1
 *    - On success with n > 0, *result >= n
 *    - On failure, return value == AWS_OP_ERR
 */

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare inputs */
    size_t n = nondet_size_t();
    size_t result;

    /* Save snapshot of n before the call */
    size_t n_before = n;

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 3. Assert n is unchanged (frame condition) */
    assert(n == n_before);

    /* 4. Assert postconditions on both paths */
    if (ret == AWS_OP_SUCCESS) {
        /* Return value is AWS_OP_SUCCESS */
        assert(ret == 0);

        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= n */
        assert(result >= n);

        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }

        /* result must be the SMALLEST power of two >= n:
         * i.e., result/2 < n (unless result == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }

        /* result must not exceed SIZE_MAX (trivially true for size_t, but explicit) */
        assert(result <= SIZE_MAX);

        /* result must be within valid range: n <= result <= SIZE_MAX_POWER_OF_TWO * 2
         * (SIZE_MAX_POWER_OF_TWO is the largest power of two in size_t) */
        assert(result <= SIZE_MAX_POWER_OF_TWO || result == 1);
    } else {
        /* Return value is AWS_OP_ERR */
        assert(ret == -1);

        /* Overflow condition: n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
