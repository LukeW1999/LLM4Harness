#include <aws/common/math.h>
#include <stddef.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t local_result;
    size_t *result = &local_result;
    size_t old_result = *result;

    int ret_val = aws_round_up_to_power_of_two(n, result);

    if (ret_val == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(*result == 1);
        } else {
            assert(aws_is_power_of_two(*result));
            assert(*result >= n);
            assert(*result / 2 < n);
        }
        assert(n <= SIZE_MAX_POWER_OF_TWO);
    } else {
        assert(*result == old_result);
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
