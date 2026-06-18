#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = nondet_size_t();
    result = old_result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result));
        assert(result >= n);
        if (result > 1) {
            assert((result >> 1) < n);
        }
        if (n == 0) {
            assert(result == 1);
        }
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }
}
