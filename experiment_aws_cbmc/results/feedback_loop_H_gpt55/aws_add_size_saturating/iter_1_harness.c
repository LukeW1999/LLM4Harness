#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result = nondet_size_t();
    size_t old_result = result;

    int rval = aws_round_up_to_power_of_two(n, &result);

    assert(rval == AWS_OP_SUCCESS || rval == AWS_OP_ERR);

    if (n <= SIZE_MAX_POWER_OF_TWO) {
        assert(rval == AWS_OP_SUCCESS);
        assert(result >= n);
        assert(result != 0);
        assert((result & (result - 1)) == 0);
        assert(result <= SIZE_MAX_POWER_OF_TWO);
        assert(result == 1 || (result >> 1) < n);
        if (aws_is_power_of_two(n)) {
            assert(result == n);
        }
    } else {
        assert(rval == AWS_OP_ERR);
        assert(result == old_result);
    }
}
