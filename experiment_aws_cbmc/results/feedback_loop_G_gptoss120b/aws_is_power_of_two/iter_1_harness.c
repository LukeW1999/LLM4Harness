#include <assert.h>
#include <stddef.h>
#include <aws/common/common.h>
#include <aws/common/math.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* output variable */
    size_t result;
    /* give result an initial nondeterministic value to detect unchanged on failure */
    result = nondet_size_t();
    size_t old_result = result;

    /* call the function under test */
    int rc = aws_round_up_to_power_of_two(n, &result);

    /* postconditions */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(result != 0);
        assert((result & (result - 1)) == 0);

        /* result must be >= original n */
        assert(result >= n);

        /* result must be the smallest power of two >= n */
        if (n == 0) {
            assert(result == 1);
        } else {
            /* there is no smaller power of two that is >= n */
            size_t smaller = result >> 1;
            assert(smaller < n);
        }

        /* result must not exceed the largest representable power of two */
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* on error the output must be unchanged */
        assert(result == old_result);
    }

    /* validity invariants (trivially true for scalars) */
    /* No additional structure to validate */
}
