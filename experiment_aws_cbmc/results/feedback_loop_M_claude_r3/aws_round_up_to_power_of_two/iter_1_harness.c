#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare and bound inputs */
    size_t n = nondet_size_t();
    /* n is unbounded size_t — no need to restrict further since the function handles all values */

    size_t result;

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 3. Assert postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= n */
        assert(result >= n);

        /* result must be the SMALLEST power of two >= n */
        /* i.e., result/2 < n (unless result == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }

        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }

        /* result must not overflow SIZE_MAX — it's a valid size_t */
        assert(result <= SIZE_MAX);

        /* result must be <= SIZE_MAX_POWER_OF_TWO (the largest power of two in size_t) */
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* On failure (overflow), n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* result is unspecified on failure — no assertion on result */
    }
}
