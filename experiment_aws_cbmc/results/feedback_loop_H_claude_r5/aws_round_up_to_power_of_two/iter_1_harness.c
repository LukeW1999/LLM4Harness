#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare inputs */
    size_t n = nondet_size_t();
    size_t result;

    /* 2. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 3. Assert postconditions */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));
        /* result must be >= n */
        assert(result >= n);
        /* result must be the smallest power of two >= n */
        /* i.e., result/2 < n (unless result == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }
        /* n == 0 case: result must be 1 */
        if (n == 0) {
            assert(result == 1);
        }
        /* n > SIZE_MAX_POWER_OF_TWO would have returned error, so n <= SIZE_MAX_POWER_OF_TWO */
        assert(n <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* Failure: n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(ret == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* result is not written on failure, so we don't assert on it */
    }
}
