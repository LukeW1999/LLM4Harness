#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result_val;
    
    int ret = aws_round_up_to_power_of_two(n, &result_val);
    
    if (ret == AWS_OP_SUCCESS) {
        assert(aws_is_power_of_two(result_val));
        assert(result_val >= n);
        if (result_val > 1) {
            assert((result_val >> 1) < n);
        }
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
