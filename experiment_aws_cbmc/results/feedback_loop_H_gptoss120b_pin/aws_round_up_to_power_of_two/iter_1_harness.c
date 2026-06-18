#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic inputs */
    size_t n = nondet_size_t();

    /* allocate result storage */
    size_t result_val = nondet_size_t();
    size_t *result = &result_val;

    /* save old state of *result for failure case */
    size_t old_result = result_val;

    /* call function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* rc must be either success or error */
    assert(rc == AWS_OP_SUCCESS || rc == AWS_OP_ERR);

    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(*result));

        /* result must be at least 1 */
        assert(*result >= 1);

        /* result must be >= n (with n==0 case result == 1) */
        if (n == 0) {
            assert(*result == 1);
        } else {
            assert(*result >= n);
        }

        /* result cannot exceed the largest power‑of‑two that fits in size_t */
        assert(*result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* on failure the output must be unchanged */
        assert(*result == old_result);

        /* failure occurs only when n is too large (and not zero) */
        assert(n > SIZE_MAX_POWER_OF_TWO);
    }

    /* n is passed by value, it must remain unchanged */
    /* (no need to assert as n is a local copy, but we keep the invariant) */
    (void)n; /* silence unused‑variable warnings */
}
