#include <aws/common/math.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;

    /* Non-deterministically initialize inputs */
    n = nondet_size_t();
    result = nondet_size_t();
    old_result = result;

    /* Assume the output pointer is valid (we use address of local variable) */

    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            /* result must be a power of two and >= n */
            assert(result >= n);
            assert((result & (result - 1)) == 0);
            /* Ensure result is within representable range */
            assert(result <= SIZE_MAX_POWER_OF_TWO);
        }
    } else {
        /* On failure, result must be unchanged */
        assert(result == old_result);
    }
}
