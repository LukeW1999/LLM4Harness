#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result;
    /* Save old value to check unchanging on failure */
    old_result = result;

    int rval = aws_round_up_to_power_of_two(n, &result);

    if (rval == AWS_OP_SUCCESS) {
        /* *result must be a power of two and >= n (with special case n == 0) */
        assert(result != 0);
        assert((result & (result - 1)) == 0); /* power of two */
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
            /* Check that (result >> 1) < n, i.e., it's the smallest power of two >= n */
            /* Since result is power of two, result >> 1 is the next smaller power */
            assert((result >> 1) < n);
        }
    } else {
        /* On error, n must exceed SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* *result unchanged */
        assert(result == old_result);
    }
}
