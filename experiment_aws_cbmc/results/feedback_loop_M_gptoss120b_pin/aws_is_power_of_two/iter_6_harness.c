#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= 1024);

    size_t result = 0;
    size_t old_result = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
            assert(aws_is_power_of_two(result));
            assert((result >> 1) < n);
        }
    } else {
        assert(result == old_result);
    }
}
