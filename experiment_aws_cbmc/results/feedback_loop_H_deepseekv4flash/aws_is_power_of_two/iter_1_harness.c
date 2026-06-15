#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    /* Nondeterministic inputs */
    size_t n;
    size_t *result = malloc(sizeof(*result));
    __CPROVER_assume(result != NULL);
    *result = nondet_size_t();
    size_t old_result = *result;

    /* Call the function under proof */
    int rval = aws_round_up_to_power_of_two(n, result);

    /* Postconditions */
    if (rval == AWS_OP_SUCCESS) {
        /* The result is the smallest power of two >= n */
        if (n == 0) {
            assert(*result == 1);
        } else {
            assert(*result >= n);
            assert(*result != 0);
            assert((*result & (*result - 1)) == 0); // power of two
        }
    } else {
        /* Overflow: n > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* On failure, *result is unchanged */
        assert(*result == old_result);
    }

    free(result);
}
