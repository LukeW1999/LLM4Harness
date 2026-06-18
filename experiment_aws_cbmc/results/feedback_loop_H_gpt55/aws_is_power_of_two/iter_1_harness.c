#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    int return_code = aws_round_up_to_power_of_two(n, &result);

    assert(return_code == AWS_OP_SUCCESS || return_code == AWS_OP_ERR);

    if (n <= SIZE_MAX_POWER_OF_TWO) {
        assert(return_code == AWS_OP_SUCCESS);
    } else {
        assert(return_code == AWS_OP_ERR);
    }

    if (return_code == AWS_OP_SUCCESS) {
        assert(result != 0);
        assert((result & (result - 1)) == 0);
        assert(result >= n);
        assert(result <= SIZE_MAX_POWER_OF_TWO);

        if (n == 0) {
            assert(result == 1);
        }

        if (result > 1) {
            assert((result >> 1) < n);
        }
    } else {
        assert(result == old_result);
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
