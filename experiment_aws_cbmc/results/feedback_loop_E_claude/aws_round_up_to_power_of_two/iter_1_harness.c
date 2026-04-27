#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_round_up_to_power_of_two
 *
 * Assertions:
 *   - If n == 0, result == 1 and returns AWS_OP_SUCCESS
 *   - If n > SIZE_MAX_POWER_OF_TWO, returns AWS_OP_ERR
 *   - On success, *result is a power of two
 *   - On success, *result >= n
 *   - On success, *result is the smallest power of two >= n
 */
void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();

    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= n */
        assert(result >= n);

        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }

        /* result must be the smallest power of two >= n:
         * i.e., result/2 < n (unless result == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }

        /* n must not exceed SIZE_MAX_POWER_OF_TWO for success */
        assert(n <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* On failure, n must be > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
