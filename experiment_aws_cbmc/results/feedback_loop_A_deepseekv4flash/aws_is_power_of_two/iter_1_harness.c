#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t old_result;

    /* Non-deterministic input */
    n = nondet_size_t();
    result = nondet_size_t();
    old_result = result;

    /* Preconditions: result pointer must be valid */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&result, sizeof(result)));

    /* Call the function */
    int rval = aws_round_up_to_power_of_two(n, &result);

    /* Postconditions */
    if (rval == AWS_OP_SUCCESS) {
        /* On success, result is the smallest power of two >= n */
        assert(result >= n);
        assert(aws_is_power_of_two(result));
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result <= SIZE_MAX_POWER_OF_TWO);
        }
    } else {
        /* On failure, result is unchanged */
        assert(result == old_result);
        /* Failure occurs only when n > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
