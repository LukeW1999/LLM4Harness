#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();

    /* output pointer */
    size_t result;
    size_t old_result = result; /* save old value */

    /* Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* Postconditions for success path */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= n, with special case n == 0 */
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
        }
    } else {
        /* On failure, result must be unchanged */
        assert(result == old_result);

        /* Failure occurs only when n is too large to round up without overflow */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }

    /* The pointer itself does not change (trivially true) */
    assert(&result == &result);
}
