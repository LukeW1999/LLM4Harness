#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* nondeterministic initial value for result */
    size_t result = nondet_size_t();
    size_t old_result = result; /* save old value for failure case */

    int ret = aws_round_up_to_power_of_two(n, &result);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= original n */
        assert(result >= n);

        if (n == 0) {
            /* special case: n == 0 yields result == 1 */
            assert(result == 1);
        } else {
            /* result is the smallest power of two >= n */
            assert((result >> 1) < n);
        }
    } else {
        /* failure occurs only when n is too large */
        assert(n > SIZE_MAX_POWER_OF_TWO);

        /* result must be unchanged on failure */
        assert(result == old_result);
    }
}
