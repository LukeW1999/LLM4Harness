#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* allocate result storage and remember old value */
    size_t old_result = nondet_size_t();
    size_t result_val = old_result;
    size_t *result = &result_val;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));

        /* result must be >= n */
        assert(*result >= n);

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(*result == 1);
        } else {
            /* previous power of two must be < n */
            assert((*result >> 1) < n);
        }
    } else {
        /* failure case */
        assert(rc == AWS_OP_ERR);
        /* failure occurs only when n is too large to round up without overflow */
        assert(n > AWS_SIZE_MAX_POWER_OF_TWO);
        /* result must be unchanged */
        assert(*result == old_result);
    }
}
