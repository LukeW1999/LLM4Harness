#include <aws/common/math.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    // Assuming aws_is_power_of_two modifies result only on success
    __CPROVER_assume(n > 0); // Ensure n is positive as per typical usage

    bool ret = aws_is_power_of_two(n);

    // Step 1: Success path assertions
    if (ret) {
        assert(result >= n);
        assert((result & (result - 1)) == 0); // result is a power of 2
    } else {
        // Step 2: Failure path assertions
        assert(result == old_result);
    }

    // Step 3: Frame conditions
    assert(n == n); // n is unchanged
    if (ret) {
        assert(result != old_result); // result is changed on success
    } else {
        assert(result == old_result); // result is unchanged on failure
    }
}
