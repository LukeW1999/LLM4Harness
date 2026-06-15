#include <aws/common/math.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;
    int ret;

    // Initialize result to any value (to check it is unchanged on failure)
    old_result = nondet_size_t();
    result = old_result;

    // Call the function under test
    ret = aws_round_up_to_power_of_two(n, &result);

    // Postcondition assertions
    if (ret == AWS_OP_SUCCESS) {
        // The result is a power of two
        assert(result != 0);
        assert((result & (result - 1)) == 0);

        // The result is >= n
        assert(result >= n);

        // If n == 0, the result must be 1
        if (n == 0) {
            assert(result == 1);
        }

        // For n > 0, the result is the smallest power of two >= n,
        // so result / 2 < n
        if (n > 0) {
            assert(result / 2 < n);
        }
    } else { // AWS_OP_ERR
        // Failure only occurs when n is too large
        assert(n > SIZE_MAX_POWER_OF_TWO);

        // The output pointer is not written on failure
        assert(result == old_result);
    }
}
