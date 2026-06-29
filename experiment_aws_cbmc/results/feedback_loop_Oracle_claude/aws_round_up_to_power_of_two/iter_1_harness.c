#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n;
    size_t result;
    size_t result_before = result; /* capture initial value */

    int rc = aws_round_up_to_power_of_two(n, &result);

    if (n == 0) {
        /* When n == 0, should succeed and return 1 */
        assert(rc == AWS_OP_SUCCESS);
        assert(result == 1);
        assert(aws_is_power_of_two(result));
    } else if (n > SIZE_MAX_POWER_OF_TWO) {
        /* When n exceeds the largest power of two, should fail */
        assert(rc == AWS_OP_ERR);
        /* result should not have been modified */
        assert(result == result_before);
    } else {
        /* Normal case: should succeed */
        assert(rc == AWS_OP_SUCCESS);

        /* Result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* Result must be >= n */
        assert(result >= n);

        /* Result must be the smallest power of two >= n:
         * i.e., result/2 < n (unless result == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }
    }
}
