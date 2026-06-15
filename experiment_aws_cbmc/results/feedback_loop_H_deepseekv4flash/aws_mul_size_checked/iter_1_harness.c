#include <aws/common/math.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;

    /* Nondeterministic input */
    result = nondet_size_t();
    old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* On success, *result is the smallest power of two >= n */
        assert(aws_is_power_of_two(result));
        assert(result >= n);
        if (aws_is_power_of_two(n)) {
            assert(result == n);
        }
        /* Also result must be <= SIZE_MAX_POWER_OF_TWO (no overflow) */
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* On failure, *result is unchanged */
        assert(result == old_result);
        /* Overflow must have been inevitable: n exceeds the largest representable power of two */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
