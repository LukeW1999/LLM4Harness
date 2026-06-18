#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();

    size_t old_result = nondet_size_t();
    size_t result_val = old_result;
    size_t *result = &result_val;

    int rc = aws_round_up_to_power_of_two(n, result);

    if (rc == AWS_OP_SUCCESS) {
        __CPROVER_assert(aws_is_power_of_two(*result), "result is power of two");
        __CPROVER_assert(*result >= n, "result >= n");
        if (n == 0) {
            __CPROVER_assert(*result == 1, "n == 0 yields result == 1");
        } else {
            __CPROVER_assert((*result >> 1) < n, "previous power of two < n");
        }
    } else {
        __CPROVER_assert(rc == AWS_OP_ERR, "rc indicates error");
        __CPROVER_assert(n > AWS_SIZE_MAX_POWER_OF_TWO, "n too large for rounding");
        __CPROVER_assert(*result == old_result, "result unchanged on error");
    }
}
