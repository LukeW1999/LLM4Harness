#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_round_up_to_power_of_two:
 *
 * Doxygen: "Function to find the smallest result that is power of 2 >= n.
 *           Returns AWS_OP_ERR if this cannot be done without overflow"
 *
 * Analysis:
 * 1. Changed fields on success: *result is set to the smallest power of 2 >= n
 * 2. Unchanged: n is not modified (input only)
 * 3. Failure path: returns AWS_OP_ERR when n > SIZE_MAX_POWER_OF_TWO
 * 4. Validity invariants:
 *    - On success: *result is a power of two
 *    - On success: *result >= n
 *    - On success: *result is the SMALLEST such power of two
 *    - On failure: *result is not written (we don't assert its value)
 */

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Declare inputs */
    size_t n = nondet_size_t();
    size_t result;

    /* 2. Snapshot inputs before call */
    size_t old_n = n;

    /* 3. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 4. Assert postconditions */

    /* Input n must not have been modified */
    assert(n == old_n);

    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));

        /* result must be >= n */
        assert(result >= n);

        /* result must be the SMALLEST power of two >= n:
         * i.e., result/2 < n (unless result == 1, which covers n == 0 and n == 1) */
        if (result > 1) {
            assert((result >> 1) < n);
        }

        /* Special case: n == 0 => result == 1 */
        if (n == 0) {
            assert(result == 1);
        }

        /* result must not exceed SIZE_MAX (trivially true for size_t, but be explicit) */
        assert(result <= SIZE_MAX);

        /* result must be a valid power of two (double-check with the library predicate) */
        assert(aws_is_power_of_two(result));

    } else {
        /* ret must be AWS_OP_ERR */
        assert(ret == AWS_OP_ERR);

        /* Overflow condition: n must have been > SIZE_MAX_POWER_OF_TWO */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }
}
