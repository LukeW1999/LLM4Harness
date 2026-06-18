#include <assert.h>
#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

extern size_t nondet_size_t(void);

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();

    size_t old_n = n;
    size_t old_result = result;

    int rc = aws_round_up_to_power_of_two(n, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert(aws_is_power_of_two(result));
        if (old_n == 0) {
            assert(result == 1);
        }
    } else {
        assert(result == old_result);
    }

    assert(n == old_n);
}
