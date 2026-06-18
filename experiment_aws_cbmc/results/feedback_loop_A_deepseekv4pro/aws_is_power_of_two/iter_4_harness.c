#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare inputs */
    size_t n = nondet_size_t();

    /* 2. Declare output pointer and initial value (to check failure leaves it unchanged) */
    size_t result;
    size_t old_result = nondet_size_t();
    result = old_result;  /* initial value */

    size_t *r = &result;

    /* 3. No restrictive preconditions – n can be any size_t */

    /* 4. Call the function under test */
    int ret = aws_round_up_to_power_of_two(n, r);

    /* 5. Postconditions:
     *    - On success: *r is the smallest power of two >= n,
     *      and n <= SIZE_MAX_POWER_OF_TWO.
     *    - On failure: n > SIZE_MAX_POWER_OF_TWO, *r unchanged,
     *      and return value is AWS_OP_ERR.
     */

    if (ret == AWS_OP_SUCCESS) {
        /* Success path */
        assert(aws_is_power_of_two(result));

        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);               /* result must be >= n */
            assert((result >> 1) < n);         /* smallest such power of two */
        }

        /* Overflow cannot have occurred */
        assert(n <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* Failure path */
        assert(ret == AWS_OP_ERR);

        /* The spec states error only when overflow would be unavoidable */
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* Output must not be modified on error */
        assert(result == old_result);
    }
}
