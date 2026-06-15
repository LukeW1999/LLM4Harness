#include <aws/common/math.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;

    n = nondet_size_t();
    result = nondet_size_t();
    old_result = result;

    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
            assert((result & (result - 1)) == 0);
            size_t max_power_of_two = (size_t)1 << (sizeof(size_t) * 8 - 1);
            assert(result <= max_power_of_two);
        }
    } else {
        assert(result == old_result);
    }
}
