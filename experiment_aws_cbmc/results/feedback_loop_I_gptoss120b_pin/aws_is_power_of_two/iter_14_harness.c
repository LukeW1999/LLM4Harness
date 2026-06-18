#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();

    size_t result = 0;

    size_t old_n = n;
    size_t old_val = result;

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result));
        assert(result >= old_n);
        if (old_n == 0) {
            assert(result == 1);
        }
    } else {
        assert(ret == AWS_OP_ERR);
        assert(result == old_val);
    }

    assert(n == old_n);
}
