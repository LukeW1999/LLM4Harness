#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare inputs */
    size_t n;
    size_t result;

    /* n is unconstrained (all values valid as inputs) */

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 3. Assert postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* On success, result must be a power of two */
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

        /* result must not overflow: it must be <= SIZE_MAX_POWER_OF_TWO */
        assert(result <= SIZE_MAX_POWER_OF_TWO);

        /* n must have been <= SIZE_MAX_POWER_OF_TWO for success */
        assert(n <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* On failure (overflow), n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(ret == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* result is not written on failure — we cannot assert its value
           since it was uninitialized, but we can assert the error condition */
    }
}
