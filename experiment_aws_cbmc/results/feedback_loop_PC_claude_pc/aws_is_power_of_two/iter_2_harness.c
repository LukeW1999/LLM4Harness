#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare inputs */
    size_t n;
    __CPROVER_assume(true); /* no constraint needed */
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
    } else {
        /* Return value is AWS_OP_ERR */
        assert(ret == -1);

        /* Overflow condition: n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
