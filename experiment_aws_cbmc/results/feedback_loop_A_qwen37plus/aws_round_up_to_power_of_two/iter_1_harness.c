#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result));
        assert(result >= n);
        if (n == 0) {
            assert(result == 1);
        }
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }
}
