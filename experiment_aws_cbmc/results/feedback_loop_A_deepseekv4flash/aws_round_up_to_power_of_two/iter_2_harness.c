#include <aws/common/math.h>
#include <stddef.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    // --- Success case: small n that should succeed ---
    size_t n1 = 5;
    size_t result1;
    size_t old_result1 = result1;
    int ret1 = aws_round_up_to_power_of_two(n1, &result1);
    if (ret1 == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result1));
        assert(result1 >= n1);
        assert(result1 / 2 < n1);
        assert(n1 <= SIZE_MAX_POWER_OF_TWO);
    } else {
        assert(result1 == old_result1);
    }

    // --- Failure case: n > SIZE_MAX_POWER_OF_TWO ---
    size_t n2;
    __CPROVER_assume(n2 > SIZE_MAX_POWER_OF_TWO);
    size_t result2;
    size_t old_result2 = result2;
    int ret2 = aws_round_up_to_power_of_two(n2, &result2);
    assert(ret2 != AWS_OP_SUCCESS);
    assert(result2 == old_result2);
}
