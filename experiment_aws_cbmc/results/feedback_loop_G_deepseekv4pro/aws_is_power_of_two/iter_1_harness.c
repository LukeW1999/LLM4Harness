#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();
    size_t result;
    size_t old_result = nondet_size_t();
    
    /* Save the initial value of result to check that it is unchanged on error */
    result = old_result;
    
    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* On success, result must be a power of two */
        assert(aws_is_power_of_two(result));
        /* result must be at least n (for n==0, result==1 which is >= 0) */
        assert(result >= n);
        /* result is the smallest such power of two:
         * for n>0, the next smaller power of two is strictly less than n */
        if (n > 0) {
            assert((result >> 1) < n);
        } else {
            /* For n==0 the function explicitly sets result to 1 */
            assert(result == 1);
        }
    } else {
        /* On failure, the error must be due to overflow */
        assert(n > SIZE_MAX_POWER_OF_TWO);
        /* The output parameter must remain unchanged */
        assert(result == old_result);
    }
}
