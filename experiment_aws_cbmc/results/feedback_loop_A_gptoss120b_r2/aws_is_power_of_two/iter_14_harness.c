#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    int rc = aws_round_up_to_power_of_two(n, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result));
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
        }
    } else {
        assert(result == old_result);
        assert(n > AWS_MAX_POWER_OF_TWO);
    }
}
