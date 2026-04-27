#include <aws/common/math.h>
#include <aws/common/common.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare inputs */
    size_t n;
    __CPROVER_assume(1); /* no constraints, allow all values */

    size_t result = 0;

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 3. Assert postconditions */
    /* Return value must be either success or error */
    __CPROVER_assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR,
                     "return value is success or error");

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        __CPROVER_assert(aws_is_power_of_two(result),
                         "result is power of two on success");

        /* result must be >= n */
        __CPROVER_assert(result >= n,
                         "result >= n on success");

        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            __CPROVER_assert(result == 1,
                             "result == 1 when n == 0");
        }

        /* result must be the SMALLEST power of 2 >= n */
        if (result > 1) {
            __CPROVER_assert((result >> 1) < n,
                             "result is smallest power of two >= n");
        }

        /* n must not exceed SIZE_MAX_POWER_OF_TWO for success */
        __CPROVER_assert(n <= SIZE_MAX_POWER_OF_TWO,
                         "n <= SIZE_MAX_POWER_OF_TWO on success");

    } else {
        /* On failure, n must have been > SIZE_MAX_POWER_OF_TWO */
        __CPROVER_assert(n > SIZE_MAX_POWER_OF_TWO,
                         "n > SIZE_MAX_POWER_OF_TWO on failure");
    }
}
