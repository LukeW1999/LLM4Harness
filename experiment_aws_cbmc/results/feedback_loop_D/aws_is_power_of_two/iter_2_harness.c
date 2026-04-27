#include <aws/common/math.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        // Success path assertions
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
            assert((result & (result - 1)) == 0); // result is a power of two
        }
    } else {
        // Failure path assertions
        assert(ret == AWS_OP_ERR);
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }
}
