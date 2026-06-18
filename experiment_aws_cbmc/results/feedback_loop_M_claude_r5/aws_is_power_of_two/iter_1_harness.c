#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare and bound inputs */
    size_t n = nondet_size_t();
    /* No need to bound n — the function handles all size_t values */

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

        /* result must not overflow: result <= SIZE_MAX */
        /* (guaranteed by the function returning success only when no overflow) */
        assert(result <= SIZE_MAX);

        /* result must be a power of two >= n, so result >= n */
        assert(result >= n);
    } else {
        /* On failure (overflow), n must be > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* result is not written on failure — no assertion on result */
    }
}
