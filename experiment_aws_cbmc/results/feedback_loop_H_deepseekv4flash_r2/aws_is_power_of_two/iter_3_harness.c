#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result;
    old_result = result;

    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        assert(result != 0);
        assert((result & (result - 1)) == 0);
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
            assert((result >> 1) < n);
        }
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_result);
    }
}
