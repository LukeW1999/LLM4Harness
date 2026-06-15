#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    /* Non-deterministic inputs */
    size_t n;
    size_t *result;

    /* Preconditions: result must be a valid writable pointer */
    __CPROVER_assume(result != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(result, sizeof(size_t)));

    /* Save old value to check immutability on failure */
    size_t old_result = *result;

    /* Call the function under test */
    int rval = aws_round_up_to_power_of_two(n, result);

    /* Postconditions */
    if (n > SIZE_MAX_POWER_OF_TWO) {
        /* Overflow: function must return error and leave *result unchanged */
        assert(rval == AWS_OP_ERR);
        assert(*result == old_result);
    } else {
        /* Success: function must return success, *result is the smallest power of two >= n */
        assert(rval == AWS_OP_SUCCESS);
        if (n == 0) {
            /* Special case: smallest power of two >= 0 is 1 */
            assert(*result == 1);
        } else {
            /* General case: *result is a power of two, >= n, and no smaller power of two suffices */
            assert(aws_is_power_of_two(*result));
            assert(*result >= n);
            assert((*result >> 1) < n);
        }
    }
}
