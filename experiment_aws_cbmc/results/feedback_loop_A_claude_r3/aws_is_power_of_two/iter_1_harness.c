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
        /* If result > 1, then result/2 < n */
        if (result > 1) {
            assert((result >> 1) < n);
        }
        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }
        /* result must not overflow: result <= SIZE_MAX */
        assert(result <= SIZE_MAX);
    } else {
        /* On failure (overflow), ret must be AWS_OP_ERR */
        assert(ret == AWS_OP_ERR);
        /* n must have been > SIZE_MAX_POWER_OF_TWO */
        /* SIZE_MAX_POWER_OF_TWO = ((size_t)1) << (SIZE_BITS - 1) */
        assert(n > (((size_t)1) << (sizeof(size_t) * 8 - 1)));
    }
}
