#include <aws/common/math.h>
#include <assert.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;

    n = nondet_size_t();
    result = nondet_size_t();
    old_result = result;

    // Compute the maximum power of two that fits in a size_t
    size_t max_power_of_two = (size_t)1 << (sizeof(size_t) * 8 - 1);

    // Use a nondeterministic flag to ensure both success and failure branches are reachable
    bool success_case = nondet_bool();
    if (success_case) {
        __CPROVER_assume(n <= max_power_of_two);
    } else {
        __CPROVER_assume(n > max_power_of_two);
    }

    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            // result is a power of two and >= n
            assert(result >= n);
            assert((result & (result - 1)) == 0);
            // result does not exceed the maximum representable power of two
            assert(result <= max_power_of_two);
        }
    } else {
        // On failure, the output pointer is not modified
        assert(result == old_result);
    }
}
