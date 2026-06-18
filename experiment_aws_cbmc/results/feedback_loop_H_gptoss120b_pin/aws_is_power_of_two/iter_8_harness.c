#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_round_up_to_power_of_two_harness(void) {
    uint64_t n_raw = nondet_uint64_t();
    uint64_t result_raw = nondet_uint64_t();

    size_t n = (size_t)n_raw;
    size_t result = (size_t)result_raw;

    size_t old_result = result;
    size_t old_n = n;

    int rc = aws_round_up_to_power_of_two(n, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result));
        assert(result >= 1);
        if (old_n == 0) {
            assert(result == 1);
        } else {
            assert(result >= old_n);
        }
    } else {
        assert(result == old_result);
    }

    if (rc != AWS_OP_SUCCESS) {
        assert(old_n != 0);
        assert(old_n > AWS_MAX_POWER_OF_TWO);
    } else {
        assert(!(old_n > AWS_MAX_POWER_OF_TWO));
    }
}
